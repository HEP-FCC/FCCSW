#include "FastSimActions.h"
#include "GeantGeneral/TrackingAction.h"

FastSimActions::FastSimActions(): G4VUserActionInitialization() {}

FastSimActions::~FastSimActions() {}

void FastSimActions::Build() const {
   SetUserAction(new TrackingAction);
}
