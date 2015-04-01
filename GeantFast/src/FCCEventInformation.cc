#include "FCCEventInformation.hh"

FCCEventInformation::FCCEventInformation() :fDoSmearing(true)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
FCCEventInformation::FCCEventInformation(G4bool aSmear): fDoSmearing(aSmear)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
FCCEventInformation::~FCCEventInformation(){}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCEventInformation::SetDoSmearing(G4bool aSmear)
{
   fDoSmearing = aSmear;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool FCCEventInformation::GetDoSmearing()
{
   return fDoSmearing;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void FCCEventInformation::Print() const
{
   G4cout<<"FCCEventInformation: "<<G4endl
         <<"do smearing: "<<fDoSmearing<<G4endl;
}
