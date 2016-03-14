#include "CreateSampleJet.h"

DECLARE_COMPONENT(CreateSampleJet)

CreateSampleJet::CreateSampleJet(const std::string& name, ISvcLocator* svcLoc) :
		GaudiAlgorithm(name, svcLoc)

{
  declareOutput("podioJets", m_jethandle);
}

StatusCode CreateSampleJet::initialize() {

	if (GaudiAlgorithm::initialize().isFailure())
		return StatusCode::FAILURE;

	return StatusCode::SUCCESS;
}

StatusCode CreateSampleJet::execute() {
  fcc::JetCollection* jets = new fcc::JetCollection();
  fcc::LorentzVector lv1;
  lv1.Px  = 20.;
  lv1.Py  = 20. ;
  lv1.Mass = 125;
  lv1.Pz   = 0.;
  fcc::Jet j1 = jets->create();
  fcc::BareJet& core = j1.Core();
  core.P4 = lv1;
  m_jethandle.put(jets);

  return StatusCode::SUCCESS;
}

StatusCode CreateSampleJet::finalize() {
	if (GaudiAlgorithm::finalize().isFailure())
		return StatusCode::FAILURE;

	return StatusCode::SUCCESS;
}
