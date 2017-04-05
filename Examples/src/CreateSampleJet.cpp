#include "CreateSampleJet.h"
#include "datamodel/JetCollection.h"

DECLARE_COMPONENT(CreateSampleJet)

CreateSampleJet::CreateSampleJet(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("podioJets", m_jethandle, "Jet collection (output)");
}

StatusCode CreateSampleJet::initialize() {

  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

StatusCode CreateSampleJet::execute() {
  auto jets = new fcc::JetCollection();
  fcc::LorentzVector lv1;
  lv1.px = 20.;
  lv1.py = 20.;
  lv1.mass = 125;
  lv1.pz = 0.;
  fcc::Jet j1 = jets->create();
  j1.p4(lv1);
  m_jethandle.put(jets);

  return StatusCode::SUCCESS;
}

StatusCode CreateSampleJet::finalize() {
  if (GaudiAlgorithm::finalize().isFailure()) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}
