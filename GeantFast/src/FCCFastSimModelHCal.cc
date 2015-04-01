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
