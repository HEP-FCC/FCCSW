#include "TestGeometry/TestKillProcess.h"

namespace test {

TestKillProcess::TestKillProcess(const std::string& aName, G4ProcessType aType) : G4VProcess(aName, aType) {}

G4VParticleChange* TestKillProcess::PostStepDoIt(const G4Track& aTrack, const G4Step&) {
  G4VParticleChange* finalState = new G4VParticleChange();
  finalState->Initialize(aTrack);
  finalState->ClearDebugFlag();
  finalState->ProposeLocalEnergyDeposit(aTrack.GetTotalEnergy());
  finalState->SetNumberOfSecondaries(0);
  finalState->ProposeTrackStatus(fStopAndKill);
  return finalState;
}
}
