#include "TestGeometry/PhysicsList.h"

//Geant4
#include "G4LeptonConstructor.hh"
#include "G4ProcessManager.hh"
#include "G4Electron.hh"

namespace test {

PhysicsList::PhysicsList(): G4VModularPhysicsList(), m_process() {}

PhysicsList::~PhysicsList() {}

void PhysicsList::ConstructParticle() {
  G4LeptonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

void PhysicsList::ConstructProcess() {
  G4VUserPhysicsList::AddTransportation();
  // process added for electrons only
  G4ParticleDefinition* particle = G4Electron::ElectronDefinition();
  G4ProcessManager* process_manager = particle->GetProcessManager();
  process_manager->AddDiscreteProcess(&m_process);
}
}
