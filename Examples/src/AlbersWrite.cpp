#include "AlbersWrite.h"
#include "DataObjects/LorentzVector.h"

DECLARE_COMPONENT(AlbersWrite)

AlbersWrite::AlbersWrite(const std::string& name, ISvcLocator* svcLoc) :
		GaudiAlgorithm(name, svcLoc)

{
  declareOutput("albersJets", m_jethandle);
}

StatusCode AlbersWrite::initialize() {

	if (GaudiAlgorithm::initialize().isFailure())
		return StatusCode::FAILURE;

	return StatusCode::SUCCESS;
}

StatusCode AlbersWrite::execute() {
  JetCollection* jets = new JetCollection();
  LorentzVector lv1;
  lv1.Phi  = 0;
  lv1.Eta  = 1 ;
  lv1.Mass = 125;
  lv1.Pt   = 50.;  
  JetHandle& j1 = jets->create(); 
  BareJet core;
  core.P4 = lv1;
  j1.setCore(core);
  m_jethandle.put(jets);

  return StatusCode::SUCCESS;
}

StatusCode AlbersWrite::finalize() {
	if (GaudiAlgorithm::finalize().isFailure())
		return StatusCode::FAILURE;

	return StatusCode::SUCCESS;
}
