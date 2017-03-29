#include "TestGeometry/TestPhysicsList.h"

// Geant4
#include "G4Electron.hh"
#include "G4LeptonConstructor.hh"
#include "G4ProcessManager.hh"

namespace test {

TestPhysicsList::TestPhysicsList() : G4VModularPhysicsList(), m_process() {}

TestPhysicsList::~TestPhysicsList() {}

void TestPhysicsList::ConstructParticle() {
  G4LeptonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

void TestPhysicsList::ConstructProcess() {
  G4VUserPhysicsList::AddTransportation();
  // process added for electrons only
  G4ParticleDefinition* particle = G4Electron::ElectronDefinition();
  G4ProcessManager* process_manager = particle->GetProcessManager();
  process_manager->AddDiscreteProcess(&m_process);
}
}
