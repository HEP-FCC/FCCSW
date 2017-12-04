#include "SimG4GeantinoDeposits.h"

// Geant4
#include "G4VModularPhysicsList.hh"
#include "G4Geantino.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VProcess.hh"
#include "G4VContinuousProcess.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessType.hh"


class GeantinoEnergyDepositProcess: public G4VContinuousProcess {
  public:
  GeantinoEnergyDepositProcess(const G4String& processName="GeantinoDepositProcess",
      G4ProcessType type=fUserDefined)
      : G4VContinuousProcess(processName, type) {
  }

  G4VParticleChange* AlongStepDoIt(const G4Track& aTrack,
     const G4Step&) {
    aParticleChange.Initialize(aTrack);
    G4double kinEnergyStart = aTrack.GetKineticEnergy();  
    // energy deposit constant for simplicity
    G4double edepo = kinEnergyStart * 0.01;
    aParticleChange.ClearDebugFlag();
    aParticleChange.ProposeLocalEnergyDeposit( edepo );
    aParticleChange.SetNumberOfSecondaries(0);
    // geantino energy unchanged for simplicity
    aParticleChange.ProposeEnergy( kinEnergyStart);
    return &aParticleChange;
  }

  G4double GetContinuousStepLimit(const G4Track&,
      G4double,
      G4double currentMinimumStep,
      G4double&) {
    return currentMinimumStep;
  }
};

class GeantinoEnergyDepositList : public G4VModularPhysicsList {
  void ConstructProcess() {
    theParticleIterator->reset();
      while( (*theParticleIterator)() ){
        G4ParticleDefinition* particle = theParticleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();
        G4String particleName = particle->GetParticleName();
        GeantinoEnergyDepositProcess* mpro   = new GeantinoEnergyDepositProcess();
        pmanager->AddContinuousProcess(mpro);
      }
      // Define transportation process
      AddTransportation();
  }

  void ConstructParticle() {
    G4Geantino::GeantinoDefinition();
  }

  void SetCuts() {
    //   the G4VUserPhysicsList::SetCutsWithDefault() method sets 
    //   the default cut value for all particle types 
    SetCutsWithDefault();   
  }
};


DECLARE_TOOL_FACTORY(SimG4GeantinoDeposits)

SimG4GeantinoDeposits::SimG4GeantinoDeposits(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  AlgTool(aType, aName, aParent) {
  declareInterface<ISimG4PhysicsList>(this);
}

StatusCode SimG4GeantinoDeposits::initialize() {
  return AlgTool::initialize();
}

StatusCode SimG4GeantinoDeposits::finalize() {
  return AlgTool::finalize();
}

G4VModularPhysicsList* SimG4GeantinoDeposits::physicsList() {
   // ownership passed to SimG4Svc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  return new GeantinoEnergyDepositList;
}

