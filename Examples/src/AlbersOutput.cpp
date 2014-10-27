#include "AlbersOutput.h"
#include "TFile.h"
#include "FWCore/AlbersDataSvc.h"

DECLARE_COMPONENT(AlbersOutput)

AlbersOutput::AlbersOutput(const std::string& name, ISvcLocator* svcLoc) :
    GaudiAlgorithm(name, svcLoc),m_first(true)
{
  declareProperty("Filename", m_filename="output.root",
                  "Name of the file to create");
}

StatusCode AlbersOutput::initialize() {
	if (GaudiAlgorithm::initialize().isFailure())
		return StatusCode::FAILURE;
        // check whether we have the AlbersEvtSvc active
        m_albersDataSvc = dynamic_cast<AlbersDataSvc*>(evtSvc().get());
        if (0==m_albersDataSvc) return StatusCode::FAILURE;
	m_file = new TFile(m_filename.c_str(),"RECREATE","data file");
	m_datatree  = new TTree("events","Events tree");
	m_metadatatree = new TTree("metadata", "Metadata tree");
        m_registry = m_albersDataSvc->getRegistry();
	return StatusCode::SUCCESS;
}

StatusCode AlbersOutput::execute() {
  // for now assume identical content for every event
  // register for writing
  for (auto& i : m_albersDataSvc->getCollections()){
    // TODO: we need the class name 
    if (m_first){  
      m_datatree->Branch(i.first.c_str(), "vector<Jet>", i.second->_getRawBuffer());
    }
    else {
      m_datatree->SetBranchAddress(i.first.c_str(),i.second->_getRawBuffer());
    }
    i.second->prepareForWrite(m_registry);  
  }
  m_first = false;
  m_datatree->Fill();
  return StatusCode::SUCCESS;
}

StatusCode AlbersOutput::finalize() {
	if (GaudiAlgorithm::finalize().isFailure())
		return StatusCode::FAILURE;
	m_metadatatree->Branch("Registry",m_registry);
	m_metadatatree->Fill();
	m_file->Write();
	m_file->Close();
	return StatusCode::SUCCESS;
}
