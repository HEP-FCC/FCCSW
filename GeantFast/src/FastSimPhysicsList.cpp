#include "FastSimPhysicsList.h"
#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include <iomanip>

#include "G4FastSimulationManagerProcess.hh"

#include "G4Decay.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

PhysicsList::PhysicsList():  G4VUserPhysicsList()
{
   SetVerboseLevel(1);
   defaultCutValue = 0.1*m;
}

PhysicsList::~PhysicsList()
{}

void PhysicsList::ConstructParticle()
{
   // In this method, static member functions should be called
   // for all particles which you want to use.
   // This ensures that objects of these particle types will be
   // created in the program.

   ConstructBosons();
   ConstructLeptons();
   ConstructMesons();
   ConstructBaryons();
   ConstructIons();
}

void PhysicsList::ConstructBosons()
{
   G4Geantino::GeantinoDefinition();
   G4ChargedGeantino::ChargedGeantinoDefinition();

   G4Gamma::GammaDefinition();
   G4OpticalPhoton::OpticalPhotonDefinition();
}

void PhysicsList::ConstructLeptons()
{
   G4LeptonConstructor pConstructor;
   pConstructor.ConstructParticle();
}

void PhysicsList::ConstructMesons()
{
   G4MesonConstructor pConstructor;
   pConstructor.ConstructParticle();
}

void PhysicsList::ConstructBaryons()
{
   G4BaryonConstructor  pConstructor;
   pConstructor.ConstructParticle();
}

void PhysicsList::ConstructIons()
{
   G4IonConstructor pConstructor;
   pConstructor.ConstructParticle();
}

void PhysicsList::ConstructProcess()
{
   AddTransportation();
   AddParameterisation();

   ConstructGeneral();
}

void PhysicsList::AddTransportation()
{
   UseCoupledTransportation();
   G4VUserPhysicsList::AddTransportation();
}

void PhysicsList::ConstructEM()
{}

void PhysicsList::ConstructGeneral()
{
   G4Decay* theDecayProcess = new G4Decay();
   theParticleIterator->reset();
   while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      if (theDecayProcess->IsApplicable(*particle)) {
         pmanager ->AddProcess(theDecayProcess);
         // set ordering for PostStepDoIt and AtRestDoIt
         pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
         pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
      }
   }
}

void PhysicsList::AddParameterisation()
{
   G4FastSimulationManagerProcess* fastSimProcess = new G4FastSimulationManagerProcess("G4FSMP");

   theParticleIterator->reset();
   while( (*theParticleIterator)() )
   {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      pmanager->AddDiscreteProcess(fastSimProcess);

      pmanager->DumpInfo();
   }
}

void PhysicsList::SetCuts()
{
   ServiceHandle<IMessageSvc> msgh("MessageSvc","PhysicsList");
   MsgStream log(&(*msgh), "PhysicsList");
   log << MSG::INFO << "Setting default cuts." << endreq;
   SetCutsWithDefault();
}

