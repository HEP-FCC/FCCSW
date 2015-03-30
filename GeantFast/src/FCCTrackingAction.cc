#include "FCCTrackingAction.hh"
#include "FCCEventInformation.hh"
#include "FCCPrimaryParticleInformation.hh"
#include "FCCOutput.hh"

#include "G4ThreeVector.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"

#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4TrackingManager.hh"
#include <iomanip>
#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCTrackingAction::FCCTrackingAction() : G4UserTrackingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCTrackingAction::~FCCTrackingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
   if( aTrack->GetMomentum().perp() < 1.0*MeV ||
       std::abs(aTrack->GetMomentum().pseudoRapidity())>5.5 )
   {
      ((G4Track*)aTrack)->SetTrackStatus(fStopAndKill);
   }
   // filling data only for primary particles
   if(aTrack->GetParentID()) return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
   if ( aTrack->GetTrackStatus() == fStopAndKill && aTrack->GetParentID()==0 )
   {
      FCCPrimaryParticleInformation* info = (FCCPrimaryParticleInformation*)aTrack->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation();
         FCCOutput::Instance()->SaveTrack(FCCOutput::eSaveMC,
                                          info->GetPartID(),
                                          info->GetPDG(),
                                          info->GetMCMomentum()/MeV );
         FCCOutput::Instance()->SaveTrack(FCCOutput::eSaveTracker,
                                          info->GetPartID(),
                                          info->GetPDG(),
                                          info->GetTrackerMomentum()/MeV,
                                          info->GetTrackerResolution(),
                                          info->GetTrackerEfficiency());
         FCCOutput::Instance()->SaveTrack(FCCOutput::eSaveEMCal,
                                          info->GetPartID(),
                                          info->GetPDG(),
                                          info->GetEMCalPosition()/mm,
                                          info->GetEMCalResolution(),
                                          info->GetEMCalEfficiency(),
                                          info->GetEMCalEnergy()/MeV);
         FCCOutput::Instance()->SaveTrack(FCCOutput::eSaveHCal,
                                          info->GetPartID(),
                                          info->GetPDG(),
                                          info->GetHCalPosition()/mm,
                                          info->GetHCalResolution(),
                                          info->GetHCalEfficiency(),
                                          info->GetHCalEnergy()/MeV);
   }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

