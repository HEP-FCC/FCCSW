#include "FCCEventAction.hh"
#include "FCCEventInformation.hh"
#include "FCCRunAction.hh"
#include "FCCOutput.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCEventAction::FCCEventAction() : G4UserEventAction(), fSmear(1)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCEventAction::FCCEventAction(G4bool aSmear)  : G4UserEventAction(), fSmear(aSmear)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCEventAction::~FCCEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCEventAction::BeginOfEventAction(const G4Event* /*aEvent*/)
{
   G4EventManager::GetEventManager()->SetUserInformation(new FCCEventInformation(fSmear));
   FCCOutput::Instance()->CreateNtuples();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCEventAction::EndOfEventAction(const G4Event* /*aEvent*/){
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
