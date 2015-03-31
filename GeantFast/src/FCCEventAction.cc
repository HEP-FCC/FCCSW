#include "FCCEventAction.hh"
#include "FCCOutput.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCEventAction::FCCEventAction() : G4UserEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCEventAction::~FCCEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCEventAction::BeginOfEventAction(const G4Event* /*aEvent*/)
{
   FCCOutput::Instance()->CreateNtuples();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCEventAction::EndOfEventAction(const G4Event* /*aEvent*/){
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
