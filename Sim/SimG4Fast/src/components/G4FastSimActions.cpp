#include "G4FastSimActions.h"

// FCCSW
#include "SimG4Fast/FastSimActions.h"

DECLARE_TOOL_FACTORY(G4FastSimActions)

G4FastSimActions::G4FastSimActions(const std::string& type, const std::string& name, const IInterface* parent) :
  AlgTool(type, name, parent) {
  declareInterface<IG4ActionTool>(this);
  declareProperty("smearing", m_smearTool);
  declarePublicTool(m_smearTool);
}

G4FastSimActions::~G4FastSimActions() {}

StatusCode G4FastSimActions::initialize() {
  if(AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_smearTool.retrieve();
  return StatusCode::SUCCESS;
}

StatusCode G4FastSimActions::finalize() {
  return AlgTool::finalize();
}

G4VUserActionInitialization* G4FastSimActions::getUserActionInitialization() {
  return new sim::FastSimActions(m_smearTool.name());
}
