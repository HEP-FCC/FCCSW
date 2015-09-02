#include "FastSimPhysics.h"

//Geant4
#include "G4FastSimulationManagerProcess.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTable.hh"

#include "G4PhysicsConstructorFactory.hh"

G4_DECLARE_PHYSCONSTR_FACTORY(FastSimPhysics);

FastSimPhysics::FastSimPhysics(): G4VPhysicsConstructor("fastsim")
{}

FastSimPhysics::FastSimPhysics(const G4String& aName): G4VPhysicsConstructor(aName)
{}

FastSimPhysics::~FastSimPhysics(){}

void FastSimPhysics::ConstructParticle()
{}
void FastSimPhysics::ConstructProcess()
{
   G4FastSimulationManagerProcess* fastSimProcess = new G4FastSimulationManagerProcess("G4FSMP");
   aParticleIterator->reset();
   while( (*aParticleIterator)() )
   {
      G4ParticleDefinition* particle = aParticleIterator->value();

      G4ProcessManager* process_manager = particle->GetProcessManager();
      process_manager->AddProcess(fastSimProcess);
      process_manager->SetProcessOrdering(fastSimProcess, idxPostStep);
      //proper way doesn't work now - no process type 8:301 (fastsim) in OrderingTable of G4PhysicsListHelper::theTable
      //RegisterProcess(fastSimProcess, particle);
   }
}
