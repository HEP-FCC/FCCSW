#include "EventAction.hh"
#include "Output.hh"

EventAction::EventAction() : G4UserEventAction()
{}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event* /*aEvent*/)
{
   Output::Instance()->CreateNtuples();
}

void EventAction::EndOfEventAction(const G4Event* /*aEvent*/)
{}
