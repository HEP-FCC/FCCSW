#include "SimG4Fast/FastSimPhysics.h"

// Geant4
#include "G4FastSimulationManagerProcess.hh"
#include "G4PhysicsConstructorFactory.hh"
#include "G4ProcessManager.hh"

namespace sim {

G4_DECLARE_PHYSCONSTR_FACTORY(FastSimPhysics);

FastSimPhysics::FastSimPhysics() : G4VPhysicsConstructor("fastsim") {}

FastSimPhysics::FastSimPhysics(const G4String& aName) : G4VPhysicsConstructor(aName) {}

FastSimPhysics::~FastSimPhysics() {}

void FastSimPhysics::ConstructParticle() {}

void FastSimPhysics::ConstructProcess() {
  G4FastSimulationManagerProcess* fastSimProcess = new G4FastSimulationManagerProcess("G4FSMP");
  aParticleIterator->reset();
  // Fast simulation manager process is available for all the particles
  while ((*aParticleIterator)()) {
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4ProcessManager* process_manager = particle->GetProcessManager();
    process_manager->AddDiscreteProcess(fastSimProcess);
  }
}
}
