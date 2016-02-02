#include "PodioWrite.h"
#include "DataObjects/LorentzVector.h"

DECLARE_COMPONENT(PodioWrite)

PodioWrite::PodioWrite(const std::string& name, ISvcLocator* svcLoc) :
		GaudiAlgorithm(name, svcLoc)

{
  declareOutput("podioJets", m_jethandle);
}

StatusCode PodioWrite::initialize() {

	if (GaudiAlgorithm::initialize().isFailure())
		return StatusCode::FAILURE;

	return StatusCode::SUCCESS;
}

StatusCode PodioWrite::execute() {
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

StatusCode PodioWrite::finalize() {
	if (GaudiAlgorithm::finalize().isFailure())
		return StatusCode::FAILURE;

	return StatusCode::SUCCESS;
}
