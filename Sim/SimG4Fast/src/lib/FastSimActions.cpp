#include "SimG4Fast/FastSimActions.h"
#include "SimG4Fast/SaveParticlesTrackingAction.h"

namespace sim{
FastSimActions::FastSimActions(): G4VUserActionInitialization() {}

FastSimActions::~FastSimActions() {}

void FastSimActions::Build() const {
   SetUserAction(new SaveParticlesTrackingAction);
}
}
