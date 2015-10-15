#include "ActionInitialization.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"


ActionInitialization::ActionInitialization() : G4VUserActionInitialization(), fFileName("DefaultOutput")
{}

ActionInitialization::ActionInitialization(const G4String aOutName) : G4VUserActionInitialization(), fFileName(aOutName)
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{}

void ActionInitialization::Build() const
{
   SetUserAction(new RunAction(fFileName));
   SetUserAction(new EventAction);
   SetUserAction(new TrackingAction);
}
