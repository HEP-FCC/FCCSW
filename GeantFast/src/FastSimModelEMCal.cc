#include "FastSimModelEMCal.hh"
#include "PrimaryParticleInformation.hh"
#include "Smearer.hh"
#include "Output.hh"

#include "G4Track.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"

FastSimModelEMCal::FastSimModelEMCal(G4String aModelName, G4Region* aEnvelope, DetectorParametrisation::Parametrisation aType)
   : G4VFastSimulationModel(aModelName, aEnvelope), fCalculateParametrisation(), fParametrisation(aType)
{}

FastSimModelEMCal::FastSimModelEMCal(G4String aModelName, G4Region* aEnvelope)
   : G4VFastSimulationModel(aModelName, aEnvelope), fCalculateParametrisation(), fParametrisation(DetectorParametrisation::eCMS)
{}

FastSimModelEMCal::FastSimModelEMCal(G4String aModelName)
   : G4VFastSimulationModel(aModelName), fCalculateParametrisation(), fParametrisation(DetectorParametrisation::eCMS)
{}

FastSimModelEMCal::~FastSimModelEMCal()
{}

G4bool FastSimModelEMCal::IsApplicable(const G4ParticleDefinition& aParticleType)
{
   return &aParticleType == G4Electron::Definition() ||
       &aParticleType == G4Positron::Definition() ||
       &aParticleType == G4Gamma::Definition();
}

G4bool FastSimModelEMCal::ModelTrigger(const G4FastTrack& /*aFastTrack*/)
{
   return true;
}

void FastSimModelEMCal::DoIt(const G4FastTrack& aFastTrack,
                            G4FastStep& aFastStep)
{
   // Kill the parameterised particle:
   aFastStep.KillPrimaryTrack();
   aFastStep.ProposePrimaryTrackPathLength(0.0);
   G4double Edep = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

   // here smear according to tracker resolution
   G4ThreeVector Pos = aFastTrack.GetPrimaryTrack()->GetPosition();

   if ( !aFastTrack.GetPrimaryTrack()->GetParentID() )
   {
      G4ThreeVector Porg = aFastTrack.GetPrimaryTrack()->GetMomentum();
      G4double res = fCalculateParametrisation->GetResolution(DetectorParametrisation::eEMCAL, fParametrisation, Porg.mag());
      G4double eff = fCalculateParametrisation->GetEfficiency(DetectorParametrisation::eEMCAL, fParametrisation, Porg.mag());
      G4double Esm;
      Esm = abs(Smearer::Instance()->SmearEnergy(aFastTrack.GetPrimaryTrack(), res));
      Output::Instance()->FillHistogram(1, (Esm/MeV) / (Edep/MeV));

      ((PrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                        (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetEMCalPosition(Pos);
      ((PrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                        (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetEMCalEnergy(Esm);
      ((PrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                        (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetEMCalResolution(res);
      ((PrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                        (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetEMCalEfficiency(eff);
      aFastStep.ProposeTotalEnergyDeposited(Esm);
   }
}
