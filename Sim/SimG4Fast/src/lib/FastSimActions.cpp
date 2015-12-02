#include "SimG4Fast/FastSimActions.h"
#include "GeantGeneral/SaveParticlesTrackingAction.h"

FastSimActions::FastSimActions(): G4VUserActionInitialization() {}

FastSimActions::~FastSimActions() {}

void FastSimActions::Build() const {
   SetUserAction(new SaveParticlesTrackingAction);
}
