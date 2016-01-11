#include "G4FullSimActions.h"

// FCCSW
#include "SimG4Full/FullSimActions.h"

DECLARE_COMPONENT(G4FullSimActions)

G4FullSimActions::G4FullSimActions(const std::string& type, const std::string& name, const IInterface* parent) :
GaudiTool(type, name, parent) {
   declareInterface<IG4ActionTool>(this);
}

G4FullSimActions::~G4FullSimActions() {}

StatusCode G4FullSimActions::initialize() {
   if(GaudiTool::initialize().isFailure()) {
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}

StatusCode G4FullSimActions::finalize() {
  return GaudiTool::finalize();
}

G4VUserActionInitialization* G4FullSimActions::getUserActionInitialization() {
  return new sim::FullSimActions;
}
