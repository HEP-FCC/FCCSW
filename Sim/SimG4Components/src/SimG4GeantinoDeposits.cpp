#include "SimG4GeantinoDeposits.h"

// Geant4
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessType.hh"
#include "G4VContinuousProcess.hh"
#include "G4VModularPhysicsList.hh"
#include "G4VProcess.hh"
#include "G4VUserPhysicsList.hh"

#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

class G4ChargedGeantinoNegative : public G4ParticleDefinition {
private:
  static G4ChargedGeantinoNegative* theInstance;

private:
  G4ChargedGeantinoNegative() {}

public:
  ~G4ChargedGeantinoNegative() {}

  static G4ChargedGeantinoNegative* Definition();
  static G4ChargedGeantinoNegative* ChargedGeantinoDefinition();
  static G4ChargedGeantinoNegative* ChargedGeantino();
};

G4ChargedGeantinoNegative* G4ChargedGeantinoNegative::theInstance = 0;

G4ChargedGeantinoNegative* G4ChargedGeantinoNegative::Definition() {
  if (theInstance != 0) return theInstance;

  const G4String name = "chargedgeantinonegative";
  // search in particle table
  G4ParticleTable* pTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* anInstance = pTable->FindParticle(name);
  if (anInstance == 0) {
    // create particle
    //
    //    Arguments for constructor are as follows
    //               name             mass          width         charge
    //             2*spin           parity  C-conjugation
    //          2*Isospin       2*Isospin3       G-parity
    //               type    lepton number  baryon number   PDG encoding
    //             stable         lifetime    decay table
    //             shortlived      subType    anti_encoding
    anInstance = new G4ParticleDefinition(name, 0.0 * MeV, 0.0 * MeV, -1. * eplus, 0, 0, 0, 0, 0, 0, "geantino", 0, 0,
                                          0, true, 0.0, NULL, false, "geantino", 0);
  }
  theInstance = reinterpret_cast<G4ChargedGeantinoNegative*>(anInstance);
  return theInstance;
}

G4ChargedGeantinoNegative* G4ChargedGeantinoNegative::ChargedGeantinoDefinition() { return Definition(); }

G4ChargedGeantinoNegative* G4ChargedGeantinoNegative::ChargedGeantino() { return Definition(); }

class GeantinoEnergyDepositProcess : public G4VContinuousProcess {
public:
  GeantinoEnergyDepositProcess(const G4String& processName = "GeantinoDepositProcess",
                               G4ProcessType type = fUserDefined)
      : G4VContinuousProcess(processName, type) {}

  G4VParticleChange* AlongStepDoIt(const G4Track& aTrack, const G4Step&) {
    aParticleChange.Initialize(aTrack);
    G4double kinEnergyStart = aTrack.GetKineticEnergy();
    // energy deposit constant for simplicity
    G4double edepo = kinEnergyStart * 0.01;
    aParticleChange.ClearDebugFlag();
    aParticleChange.ProposeLocalEnergyDeposit(edepo);
    aParticleChange.SetNumberOfSecondaries(0);
    // geantino energy unchanged for simplicity
    aParticleChange.ProposeEnergy(kinEnergyStart);
    return &aParticleChange;
  }

  G4double GetContinuousStepLimit(const G4Track&, G4double, G4double currentMinimumStep, G4double&) {
    return currentMinimumStep;
  }
};

class GeantinoEnergyDepositList : public G4VModularPhysicsList {
  void ConstructProcess() {
    auto theParticleIterator = GetParticleIterator();
    theParticleIterator->reset();
    while ((*theParticleIterator)()) {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();
      GeantinoEnergyDepositProcess* mpro = new GeantinoEnergyDepositProcess();
      pmanager->AddContinuousProcess(mpro);
    }
    // Define transportation process
    AddTransportation();
  }

  void ConstructParticle() {
    G4Geantino::GeantinoDefinition();
    G4ChargedGeantino::ChargedGeantinoDefinition();
    G4ChargedGeantinoNegative::ChargedGeantinoDefinition();
  }

  void SetCuts() {
    //   the G4VUserPhysicsList::SetCutsWithDefault() method sets
    //   the default cut value for all particle types
    SetCutsWithDefault();
  }
};

DECLARE_COMPONENT(SimG4GeantinoDeposits)

SimG4GeantinoDeposits::SimG4GeantinoDeposits(const std::string& aType, const std::string& aName,
                                             const IInterface* aParent)
    : AlgTool(aType, aName, aParent) {
  declareInterface<ISimG4PhysicsList>(this);
}

StatusCode SimG4GeantinoDeposits::initialize() { return AlgTool::initialize(); }

StatusCode SimG4GeantinoDeposits::finalize() { return AlgTool::finalize(); }

G4VModularPhysicsList* SimG4GeantinoDeposits::physicsList() {
  // ownership passed to SimG4Svc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  return new GeantinoEnergyDepositList;
}
