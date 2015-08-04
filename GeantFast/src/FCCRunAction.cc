#include "FCCOutput.hh"
#include "FCCRunAction.hh"
#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCRunAction::FCCRunAction(const G4String aOutName="SimpleOutput")
 : G4UserRunAction()
{
   FCCOutput::Instance()->SetFileName(aOutName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCRunAction::~FCCRunAction()
{
   delete FCCOutput::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCRunAction::BeginOfRunAction(const G4Run* aRun)
{
   FCCOutput::Instance()->StartAnalysis(aRun->GetRunID());
   FCCOutput::Instance()->CreateHistograms();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCRunAction::EndOfRunAction(const G4Run* /*aRun*/)
{
   FCCOutput::Instance()->EndAnalysis();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
