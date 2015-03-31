#include "FCCEventAction.hh"
#include "FCCOutput.hh"

FCCEventAction::FCCEventAction() : G4UserEventAction()
{}

FCCEventAction::~FCCEventAction()
{}

void FCCEventAction::BeginOfEventAction(const G4Event* /*aEvent*/)
{
   FCCOutput::Instance()->CreateNtuples();
}

void FCCEventAction::EndOfEventAction(const G4Event* /*aEvent*/)
{}
