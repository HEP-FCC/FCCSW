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
  lv1.Px  = 20.;
  lv1.Py  = 20. ;
  lv1.Mass = 125;
  lv1.Pz   = 0.;  
  JetHandle j1 = jets->create(); 
  BareJet& core = j1.mod().Core;
  core.P4 = lv1;
  m_jethandle.put(jets);

  return StatusCode::SUCCESS;
}

StatusCode AlbersWrite::finalize() {
	if (GaudiAlgorithm::finalize().isFailure())
		return StatusCode::FAILURE;

	return StatusCode::SUCCESS;
}
