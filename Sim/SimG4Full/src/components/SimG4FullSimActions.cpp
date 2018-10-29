#include "SimG4FullSimActions.h"

// FCCSW
#include "SimG4Full/FullSimActions.h"

DECLARE_COMPONENT(SimG4FullSimActions)

SimG4FullSimActions::SimG4FullSimActions(const std::string& type, const std::string& name, const IInterface* parent)
    : AlgTool(type, name, parent) {
  declareInterface<ISimG4ActionTool>(this);
}

SimG4FullSimActions::~SimG4FullSimActions() {}

StatusCode SimG4FullSimActions::initialize() {
  if (AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4FullSimActions::finalize() { return AlgTool::finalize(); }

G4VUserActionInitialization* SimG4FullSimActions::userActionInitialization() {
  return new sim::FullSimActions(m_enableHistory, m_energyCut, m_selectTaggedOnly);
}
