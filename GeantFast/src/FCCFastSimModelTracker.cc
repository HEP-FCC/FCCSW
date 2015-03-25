//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#include "FCCFastSimModelTracker.hh"
#include "FCCEventInformation.hh"
#include "FCCPrimaryParticleInformation.hh"
#include "FCCSmearer.hh"
#include "FCCOutput.hh"

#include "G4Track.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "g4root.hh"

#include "Randomize.hh"

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"

#include "G4PathFinder.hh"
#include "G4FieldTrack.hh"
#include "G4FieldTrackUpdator.hh"
#include "G4SystemOfUnits.hh"

FCCFastSimModelTracker::FCCFastSimModelTracker(G4String aModelName, G4Region* aEnvelope, FCCDetectorParametrisation::Parametrisation aType)
   : G4VFastSimulationModel(aModelName, aEnvelope), fCalculateParametrisation(), fParametrisation(aType)
{}

FCCFastSimModelTracker::FCCFastSimModelTracker(G4String aModelName, G4Region* aEnvelope)
   : G4VFastSimulationModel(aModelName, aEnvelope), fCalculateParametrisation(), fParametrisation(FCCDetectorParametrisation::eCMS)
{}

FCCFastSimModelTracker::FCCFastSimModelTracker(G4String aModelName)
   : G4VFastSimulationModel(aModelName), fCalculateParametrisation(), fParametrisation(FCCDetectorParametrisation::eCMS)
{}

FCCFastSimModelTracker::~FCCFastSimModelTracker()
{}

G4bool FCCFastSimModelTracker::IsApplicable(const G4ParticleDefinition& aParticleType)
{
   return  aParticleType.GetPDGCharge() != 0;
}

G4bool FCCFastSimModelTracker::ModelTrigger(const G4FastTrack& /*aFastTrack*/)
{
   return true;
}

void FCCFastSimModelTracker::DoIt(const G4FastTrack& aFastTrack,
                            G4FastStep& aFastStep)
{
// ================
   G4ThreeVector spin        = aFastTrack.GetPrimaryTrack()->GetPolarization() ;
   G4FieldTrack  theFieldTrack = G4FieldTrack( aFastTrack.GetPrimaryTrack()->GetPosition(),
                                             aFastTrack.GetPrimaryTrack()->GetMomentumDirection(),
                                             0.0,
                                             aFastTrack.GetPrimaryTrack()->GetKineticEnergy(),
                                             aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetDefinition()->GetPDGMass(),
                                             0.0,    // UNUSED: aFastTrack.GetPrimaryTrack().GetVelocity(),
                                             aFastTrack.GetPrimaryTrack()->GetGlobalTime(), // Lab.
                                             aFastTrack.GetPrimaryTrack()->GetProperTime(), // Part.
                                             &spin) ;

    G4double retSafety= -1.0;
    ELimited retStepLimited;
    G4FieldTrack endTrack('a') ;
    G4double  currentMinimumStep= 10*m; // change that to sth connected to particle momentum !!!!!
    G4PathFinder* fPathFinder=  G4PathFinder::GetInstance();
    /*G4double lengthAlongCurve = */ fPathFinder->ComputeStep( theFieldTrack,
                                                    currentMinimumStep,
                                                    0,
                                                    aFastTrack.GetPrimaryTrack()->GetCurrentStepNumber(),
                                                    retSafety,
                                                    retStepLimited,
                                                    endTrack,
                                                    aFastTrack.GetPrimaryTrack()->GetVolume() ) ;
   aFastStep.ProposePrimaryTrackFinalPosition( endTrack.GetPosition() );

// ================

   // smear according to tracker resolution
   G4ThreeVector Porg = aFastTrack.GetPrimaryTrack()->GetMomentum();
   if ( !aFastTrack.GetPrimaryTrack()->GetParentID() )
   {
      FCCEventInformation* info = (FCCEventInformation*) G4EventManager::GetEventManager()->GetUserInformation();
      if ( info->GetDoSmearing() )
      {
         G4double res = fCalculateParametrisation->GetResolution(FCCDetectorParametrisation::eTRACKER, fParametrisation, Porg.mag());
         G4double eff = fCalculateParametrisation->GetEfficiency(FCCDetectorParametrisation::eTRACKER, fParametrisation, Porg.mag());
         G4ThreeVector Psm;
         if( info->GetSavePerigee() )
            Psm = FCCSmearer::Instance()->SmearMomentum(aFastTrack.GetPrimaryTrack(), res, FCCOutput::eSaveTracker);
         else
            Psm = FCCSmearer::Instance()->SmearMomentum(aFastTrack.GetPrimaryTrack(), res);
         FCCOutput::Instance()->FillHistogram(0,((Psm.mag()/MeV) / (Porg.mag()/MeV)) );
         // setting values of Psm, res and eff
         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetTrackerMomentum(Psm);
         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetTrackerResolution(res);
         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetTrackerEfficiency(eff);
      }
      else
      {
         // setting a value of Porg
         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetTrackerMomentum(Porg);
      }
   }
}
