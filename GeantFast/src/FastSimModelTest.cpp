#include "FastSimModelTest.h"

// #include "G4Track.hh"
// #include "G4Event.hh"
// #include "G4RunManager.hh"
// #include "g4root.hh"

// #include "Randomize.hh"
// #include "G4SystemOfUnits.hh"

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"


FastSimModelTest::FastSimModelTest(G4String aModelName, G4Region* aEnvelope)
   : G4VFastSimulationModel(aModelName, aEnvelope)
{
   // StatusCode sc= serviceLocator()->getService("MessageSvc", m_msgSvc, true);
}

FastSimModelTest::FastSimModelTest(G4String aModelName)
   : G4VFastSimulationModel(aModelName)
{}

FastSimModelTest::~FastSimModelTest()
{}

G4bool FastSimModelTest::IsApplicable(const G4ParticleDefinition& aParticleType)
{
   std::cout<<" ________CHECK IF: Test model applicable _________"<<std::endl;
   return &aParticleType == G4Electron::Definition() ||
       &aParticleType == G4Positron::Definition() ||
       &aParticleType == G4Gamma::Definition();
}

G4bool FastSimModelTest::ModelTrigger(const G4FastTrack& /*aFastTrack*/)
{
   std::cout<<" ________CHECK IF: Test model triggered _________"<<std::endl;
   return true;
}

void FastSimModelTest::DoIt(const G4FastTrack& aFastTrack,
                            G4FastStep& aFastStep)
{
   std::cout<<" ________Test model triggered _________"<<std::endl;
   // Kill the parameterised particle:
   aFastStep.KillPrimaryTrack();
   aFastStep.ProposePrimaryTrackPathLength(0.0);
   G4double Edep = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();
   aFastStep.ProposeTotalEnergyDeposited(Edep);
}
