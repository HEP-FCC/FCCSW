#include "FCCActionInitialization.hh"
#include "FCCRunAction.hh"
#include "FCCEventAction.hh"
#include "FCCTrackingAction.hh"
#include "G4UIcommand.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCActionInitialization::FCCActionInitialization() : G4VUserActionInitialization(), fFileName("DefaultOutput"), fSmear(true)
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCActionInitialization::FCCActionInitialization(const G4String aOutName, const G4String aSmear) : G4VUserActionInitialization(), fFileName(aOutName),fSmear(G4UIcommand::ConvertToBool(aSmear))
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCActionInitialization::FCCActionInitialization(const G4String aOutName) : G4VUserActionInitialization(), fFileName(aOutName),fSmear(true)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCActionInitialization::~FCCActionInitialization()
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCActionInitialization::BuildForMaster() const
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCActionInitialization::Build() const
{
   SetUserAction(new FCCRunAction(fFileName));
   SetUserAction(new FCCEventAction(fSmear));
   SetUserAction(new FCCTrackingAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
