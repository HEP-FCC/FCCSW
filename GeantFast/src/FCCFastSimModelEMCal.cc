#include "FCCFastSimModelEMCal.hh"
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

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"

FCCFastSimModelEMCal::FCCFastSimModelEMCal(G4String aModelName, G4Region* aEnvelope, FCCDetectorParametrisation::Parametrisation aType)
   : G4VFastSimulationModel(aModelName, aEnvelope), fCalculateParametrisation(), fParametrisation(aType)
{}

FCCFastSimModelEMCal::FCCFastSimModelEMCal(G4String aModelName, G4Region* aEnvelope)
   : G4VFastSimulationModel(aModelName, aEnvelope), fCalculateParametrisation(), fParametrisation(FCCDetectorParametrisation::eCMS)
{}

FCCFastSimModelEMCal::FCCFastSimModelEMCal(G4String aModelName)
   : G4VFastSimulationModel(aModelName), fCalculateParametrisation(), fParametrisation(FCCDetectorParametrisation::eCMS)
{}

FCCFastSimModelEMCal::~FCCFastSimModelEMCal()
{}

G4bool FCCFastSimModelEMCal::IsApplicable(const G4ParticleDefinition& aParticleType)
{
   return &aParticleType == G4Electron::Definition() ||
       &aParticleType == G4Positron::Definition() ||
       &aParticleType == G4Gamma::Definition();
}

G4bool FCCFastSimModelEMCal::ModelTrigger(const G4FastTrack& /*aFastTrack*/)
{
   return true;
}

void FCCFastSimModelEMCal::DoIt(const G4FastTrack& aFastTrack,
                            G4FastStep& aFastStep)
{
   G4cout<<" ________EMCal model triggered _________"<<G4endl;
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
         G4double res = fCalculateParametrisation->GetResolution(FCCDetectorParametrisation::eEMCAL, fParametrisation, Porg.mag());
         G4double eff = fCalculateParametrisation->GetEfficiency(FCCDetectorParametrisation::eEMCAL, fParametrisation, Porg.mag());
         G4double Esm;
         Esm = abs(FCCSmearer::Instance()->SmearEnergy(aFastTrack.GetPrimaryTrack(), res));
         FCCOutput::Instance()->FillHistogram(1, (Esm/MeV) / (Edep/MeV));

         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetEMCalPosition(Pos);
         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetEMCalEnergy(Esm);
         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetEMCalResolution(res);
         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetEMCalEfficiency(eff);
         aFastStep.ProposeTotalEnergyDeposited(Esm);
      }
      else
      {
         ((FCCPrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                           (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetEMCalEnergy(Edep);
         aFastStep.ProposeTotalEnergyDeposited(Edep);
      }
   }
}
