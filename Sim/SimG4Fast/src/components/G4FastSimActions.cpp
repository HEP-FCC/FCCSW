#include "G4FastSimActions.h"

// FCCSW
#include "SimG4Fast/FastSimActions.h"

DECLARE_COMPONENT(G4FastSimActions)

G4FastSimActions::G4FastSimActions(const std::string& type, const std::string& name, const IInterface* parent) :
GaudiTool(type, name, parent) {
  declareInterface<IG4ActionTool>(this);
}

G4FastSimActions::~G4FastSimActions() {}

StatusCode G4FastSimActions::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

G4VUserActionInitialization* G4FastSimActions::getUserActionInitialization() {
  return new sim::FastSimActions;
}
