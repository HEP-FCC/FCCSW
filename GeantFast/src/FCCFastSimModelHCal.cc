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

#include "FCCFastSimModelHCal.hh"
#include "FCCEventInformation.hh"
#include "FCCPrimaryParticleInformation.hh"
#include "FCCSmearer.hh"
#include "FCCOutput.hh"

#include "G4Track.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "g4root.hh"

#include "Randomize.hh"
#include "G4SystemOfUnits.hh"

FCCFastSimModelHCal::FCCFastSimModelHCal(G4String aModelName, G4Region* aEnvelope, FCCDetectorParametrisation::Parametrisation aType)
   : G4VFastSimulationModel(aModelName, aEnvelope), fCalculateParametrisation(), fParametrisation(aType)
{}

FCCFastSimModelHCal::FCCFastSimModelHCal(G4String aModelName, G4Region* aEnvelope)
   : G4VFastSimulationModel(aModelName, aEnvelope), fCalculateParametrisation(), fParametrisation(FCCDetectorParametrisation::eCMS)
{}

FCCFastSimModelHCal::FCCFastSimModelHCal(G4String aModelName)
   : G4VFastSimulationModel(aModelName), fCalculateParametrisation(), fParametrisation(FCCDetectorParametrisation::eCMS)
{}

FCCFastSimModelHCal::~FCCFastSimModelHCal()
{}

G4bool FCCFastSimModelHCal::IsApplicable(const G4ParticleDefinition& aParticleType)
{
   return (aParticleType.GetQuarkContent(1) +
           aParticleType.GetQuarkContent(2) +
           aParticleType.GetQuarkContent(3) +
           aParticleType.GetQuarkContent(4) +
           aParticleType.GetQuarkContent(5) +
           aParticleType.GetQuarkContent(6) );
}

G4bool FCCFastSimModelHCal::ModelTrigger(const G4FastTrack& /*aFastTrack*/)
{
   return true;
}

void FCCFastSimModelHCal::DoIt(const G4FastTrack& aFastTrack,
                            G4FastStep& aFastStep)
{
   // Kill the parameterised particle:
   aFastStep.KillPrimaryTrack();
   aFastStep.ProposePrimaryTrackPathLength(0.0);
   G4double Edep = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

   // here Smear according to tracker resolution

   G4ThreeVector Pos = aFastTrack.GetPrimaryTrack()->GetPosition();

   if ( !aFastTrack.GetPrimaryTrack()->GetParentID() )
   {
      FCCEventInformation* info = (FCCEventInformation*) G4EventManager::GetEventManager()->GetUserInformation();
      if (info->GetDoSmearing())
      {
         G4ThreeVector Porg = aFastTrack.GetPrimaryTrack()->GetMomentum();
         G4double res = fCalculateParametrisation->GetResolution(FCCDetectorParametrisation::eHCAL, fParametrisation, Porg.mag());
         G4double eff = fCalculateParametrisation->GetEfficiency(FCCDetectorParametrisation::eHCAL, fParametrisation, Porg.mag());
         G4double Esm;
         if( info->GetSavePerigee() )
            Esm = abs(FCCSmearer::Instance()->SmearEnergy(aFastTrack.GetPrimaryTrack(), res,  FCCOutput::eSaveHCal));
         else
            Esm = abs(FCCSmearer::Instance()->SmearEnergy(aFastTrack.GetPrimaryTrack(), res));
         FCCOutput::Instance()->FillHistogram(2, (Esm/MeV) / (Edep/MeV) );

         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetHCalPosition(Pos);
         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetHCalEnergy(Esm);
         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetHCalResolution(res);
         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetHCalEfficiency(eff);
         aFastStep.ProposeTotalEnergyDeposited(Esm);
      }
      else
      {
         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetHCalEnergy(Edep);
         aFastStep.ProposeTotalEnergyDeposited(Edep);
      }
   }
}
