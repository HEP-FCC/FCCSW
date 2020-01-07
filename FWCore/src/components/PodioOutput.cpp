#include "PodioOutput.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "FWCore/PodioDataSvc.h"
#include "TFile.h"

DECLARE_COMPONENT(PodioOutput)

PodioOutput::PodioOutput(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc), m_firstEvent(true) {}

StatusCode PodioOutput::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;

  // check whether we have the PodioEvtSvc active
  m_podioDataSvc = dynamic_cast<PodioDataSvc*>(evtSvc().get());
  if (0 == m_podioDataSvc) return StatusCode::FAILURE;

  m_file = std::unique_ptr<TFile>(TFile::Open(m_filename.value().c_str(), "RECREATE", "data file"));
  // Both trees are written to the ROOT file and owned by it
  // PodioDataSvc has ownership of EventDataTree
  m_datatree = m_podioDataSvc->eventDataTree();
  m_metadatatree = new TTree("metadata", "Metadata tree");
  m_switch = KeepDropSwitch(m_outputCommands);
  return StatusCode::SUCCESS;
}

void PodioOutput::resetBranches(const std::vector<std::pair<std::string, podio::CollectionBase*>>& collections,
                                bool prepare) {
  for (auto& collNamePair : collections) {
    auto collName = collNamePair.first;
    if (m_switch.isOn(collName)) {
      // Reconnect branches and collections
      m_datatree->SetBranchAddress(collName.c_str(), collNamePair.second->getBufferAddress());
      auto colls = collNamePair.second->referenceCollections();
      if (colls != nullptr) {
        int j = 0;
        for (auto& c : (*colls)) {
          m_datatree->SetBranchAddress((collName + "#" + std::to_string(j)).c_str(), &c);
          ++j;
        }
      }
    }
    if (prepare) {
      collNamePair.second->prepareForWrite();
    }
  }
}

void PodioOutput::createBranches(const std::vector<std::pair<std::string, podio::CollectionBase*>>& collections,
                                 bool prepare) {
  for (auto& collNamePair : collections) {
    auto collName = collNamePair.first;
    // TODO: we need the class name in a better way
    std::string className(typeid(*(collNamePair.second)).name());
    size_t pos = className.find_first_not_of("0123456789");
    className.erase(0, pos);
    // demangling the namespace: due to namespace additional characters were introduced:
    // e.g. N3fcc18TrackHit
    // remove any number+char before the namespace:
    pos = className.find_first_of("0123456789");
    size_t pos1 = className.find_first_not_of("0123456789", pos);
    className.erase(0, pos1);
    // replace any numbers between namespace and class with "::"
    pos = className.find_first_of("0123456789");
    pos1 = className.find_first_not_of("0123456789", pos);
    className.replace(pos, pos1 - pos, "::");

    pos = className.find("Collection");
    className.erase(pos, pos + 10);
    std::string collClassName = "vector<" + className + "Data>";
    int isOn = 0;
    if (m_switch.isOn(collName)) {
      isOn = 1;
      m_datatree->Branch(collName.c_str(), collClassName.c_str(), collNamePair.second->getBufferAddress());
      // Create branches for collections holding relations
      auto colls = collNamePair.second->referenceCollections();
      if (colls != nullptr) {
        int j = 0;
        for (auto& c : (*colls)) {
          m_datatree->Branch((collName + "#" + std::to_string(j)).c_str(), c);
          ++j;
        }
      }
    }
    debug() << isOn << " Registering collection " << collClassName << " " << collName.c_str() << " containing type "
            << className << endmsg;
    if (prepare) {
      collNamePair.second->prepareForWrite();
    }
  }
}

StatusCode PodioOutput::execute() {
  // for now assume identical content for every event
  // register for writing
  if (m_firstEvent) {
    createBranches(m_podioDataSvc->getCollections(), true);
    createBranches(m_podioDataSvc->getReadCollections(), false);
  } else {
    resetBranches(m_podioDataSvc->getCollections(), true);
    resetBranches(m_podioDataSvc->getReadCollections(), false);
  }
  m_firstEvent = false;
  debug() << "Filling DataTree .." << endmsg;
  m_datatree->Fill();
  return StatusCode::SUCCESS;
}

/** PodioOutput::finalize
* has to happen after all algorithms that touch the data store finish.
* Here the job options are retrieved and stored to disk as a branch
* in the metadata tree.
*
*/
StatusCode PodioOutput::finalize() {
  if (GaudiAlgorithm::finalize().isFailure()) return StatusCode::FAILURE;
  //// prepare job options metadata ///////////////////////
  // retrieve the configuration of the job
  // and write it to file as vector of strings
  std::vector<std::string> config_data;
  auto jobOptionsSvc = service<IJobOptionsSvc>("JobOptionsSvc");
  auto configured_components = jobOptionsSvc->getClients();
  for (const auto& name : configured_components) {
    auto properties = jobOptionsSvc->getProperties(name);
    std::stringstream config_stream;
    for (const auto& property : *properties) {
      // sample output:
      // HepMCToEDMConverter.genparticles = "GenParticles";
      // Note that quotes are added to all property values,
      // which leads to problems with ints, lists, dicts and bools. 
      // For theses types, the quotes must be removed in postprocessing.
      config_stream << name << "." << property->name() << " = \"" << property->toString() << "\";" << std::endl;
    }
    config_data.push_back(config_stream.str());
  }
  // Some default components are not captured by the job option service
  // and have to be traversed like this. Note that Gaudi!577 will improve this.
  for (const auto* name : {"ApplicationMgr", "MessageSvc", "NTupleSvc"}) {
    std::stringstream config_stream;
    auto svc = service<IProperty>( name );
    if (!svc.isValid()) continue;
    for (const auto* property : svc->getProperties()) {
      config_stream << name << "." << property->name() << " = \"" << property->toString() << "\";" << std::endl;
    }
    config_data.push_back(config_stream.str());
  }
  //// finalize trees and file //////////////////////////////
  m_metadatatree->Branch("gaudiConfigOptions", &config_data);
  m_metadatatree->Branch("CollectionIDs", m_podioDataSvc->getCollectionIDs());
  m_metadatatree->Fill();
  m_datatree->Write();
  m_file->Write();
  m_file->Close();
  info() << "Data written to: " << m_filename.value();
  if (!m_filenameRemote.value().empty()) {
    TFile::Cp(m_filename.value().c_str(), m_filenameRemote.value().c_str(), false);
    info() << " and copied to: " << m_filenameRemote.value() << endmsg; 
  }
  return StatusCode::SUCCESS;
}
