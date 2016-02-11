#include "PodioOutput.h"
#include "TFile.h"
#include "FWCore/PodioDataSvc.h"

DECLARE_COMPONENT(PodioOutput)

PodioOutput::PodioOutput(const std::string& name, ISvcLocator* svcLoc) :
GaudiAlgorithm(name, svcLoc),m_first(true)
{
  declareProperty("filename", m_filename="output.root",
                  "Name of the file to create");
  std::vector<std::string> defaultCommands;
  defaultCommands.push_back("keep *");
  declareProperty("outputCommands", m_outputCommands=defaultCommands,
                  "A set of commands to declare which collections to keep or drop.");
}

StatusCode PodioOutput::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  // check whether we have the PodioEvtSvc active
  m_podioDataSvc = dynamic_cast<PodioDataSvc*>(evtSvc().get());
  if (0==m_podioDataSvc) return StatusCode::FAILURE;
  m_file = new TFile(m_filename.c_str(),"RECREATE","data file");
  m_datatree  = new TTree("events","Events tree");
  m_metadatatree = new TTree("metadata", "Metadata tree");
  m_switch = KeepDropSwitch(m_outputCommands);
  return StatusCode::SUCCESS;
}

StatusCode PodioOutput::execute() {
  // for now assume identical content for every event
  // register for writing
  for (auto& i : m_podioDataSvc->getCollections()){
    // TODO: we need the class name in a better way
    if (m_first) {
      std::string name( typeid(*(i.second)).name() );
      size_t  pos = name.find_first_not_of("0123456789");
      name.erase(0,pos);
      // demangling the namespace: due to namespace additional characters were introduced:
      // e.g. N3fcc18TrackHit
      // remove any number+char before the namespace:
      pos = name.find_first_of("0123456789");
      size_t pos1 = name.find_first_not_of("0123456789", pos);
      name.erase(0, pos1);
      // replace any numbers between namespace and class with "::"
      pos = name.find_first_of("0123456789");
      pos1 = name.find_first_not_of("0123456789", pos);
      name.replace(pos, pos1-pos, "::");

      pos = name.find("Collection");
      name.erase(pos,pos+10);
      std::string classname = "vector<"+name+"Data>";
      int isOn = 0;
      if( m_switch.isOn(i.first) ) {
        isOn = 1;
        m_datatree->Branch(i.first.c_str(), classname.c_str(), i.second->getBufferAddress());
      }
      debug() << isOn << " Registering collection " << classname << " " << i.first.c_str() << " containing type " << name << endmsg;
    } else {
      if ( m_switch.isOn(i.first) ) {
        m_datatree->SetBranchAddress(i.first.c_str(), i.second->getBufferAddress());
      }
    }
    i.second->prepareForWrite();
  }
  m_first = false;
  debug() << "Filling DataTree .." << endmsg;
  m_datatree->Fill();
  return StatusCode::SUCCESS;
}

StatusCode PodioOutput::finalize() {
  if (GaudiAlgorithm::finalize().isFailure())
    return StatusCode::FAILURE;
  m_metadatatree->Branch("CollectionIDs", m_podioDataSvc->getCollectionIDs());
  m_metadatatree->Fill();
  m_file->Write();
  m_file->Close();
  return StatusCode::SUCCESS;
}
