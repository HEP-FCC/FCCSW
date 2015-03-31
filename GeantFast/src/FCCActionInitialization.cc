#include "FCCActionInitialization.hh"
#include "FCCRunAction.hh"
#include "FCCEventAction.hh"
#include "FCCTrackingAction.hh"


FCCActionInitialization::FCCActionInitialization() : G4VUserActionInitialization(), fFileName("DefaultOutput")
{}

FCCActionInitialization::FCCActionInitialization(const G4String aOutName) : G4VUserActionInitialization(), fFileName(aOutName)
{}

FCCActionInitialization::~FCCActionInitialization()
{}

void FCCActionInitialization::BuildForMaster() const
{}

void FCCActionInitialization::Build() const
{
   SetUserAction(new FCCRunAction(fFileName));
   SetUserAction(new FCCEventAction);
   SetUserAction(new FCCTrackingAction);
}
