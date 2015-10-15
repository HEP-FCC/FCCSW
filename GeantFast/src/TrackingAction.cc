#include "TrackingAction.hh"
#include "PrimaryParticleInformation.hh"
#include "Output.hh"

#include "G4ThreeVector.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"

#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4TrackingManager.hh"
#include <iomanip>
#include <vector>

TrackingAction::TrackingAction() : G4UserTrackingAction()
{}

TrackingAction::~TrackingAction()
{}

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
   if( aTrack->GetMomentum().perp() < 1.0*MeV ||
       std::abs(aTrack->GetMomentum().pseudoRapidity())>5.5 )
   {
      ((G4Track*)aTrack)->SetTrackStatus(fStopAndKill);
   }
}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
   if ( aTrack->GetTrackStatus() == fStopAndKill && aTrack->GetParentID()==0 )
   {
      PrimaryParticleInformation* info = (PrimaryParticleInformation*)aTrack->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation();
         Output::Instance()->SaveTrack(Output::eSaveMC,
                                          info->GetPartID(),
                                          info->GetPDG(),
                                          info->GetMCMomentum()/MeV );
         Output::Instance()->SaveTrack(Output::eSaveTracker,
                                          info->GetPartID(),
                                          info->GetPDG(),
                                          info->GetTrackerMomentum()/MeV,
                                          info->GetTrackerResolution(),
                                          info->GetTrackerEfficiency());
         Output::Instance()->SaveTrack(Output::eSaveEMCal,
                                          info->GetPartID(),
                                          info->GetPDG(),
                                          info->GetEMCalPosition()/mm,
                                          info->GetEMCalResolution(),
                                          info->GetEMCalEfficiency(),
                                          info->GetEMCalEnergy()/MeV);
         Output::Instance()->SaveTrack(Output::eSaveHCal,
                                          info->GetPartID(),
                                          info->GetPDG(),
                                          info->GetHCalPosition()/mm,
                                          info->GetHCalResolution(),
                                          info->GetHCalEfficiency(),
                                          info->GetHCalEnergy()/MeV);
   }
}
