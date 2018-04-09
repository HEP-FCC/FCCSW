#include "SimG4UserLimitPhysicsList.h"

// FCCSW
#include "SimG4Interface/ISimG4PhysicsList.h"

// Geant4
#include "G4StepLimiterPhysics.hh"
#include "G4VModularPhysicsList.hh"

DECLARE_TOOL_FACTORY(SimG4UserLimitPhysicsList)

SimG4UserLimitPhysicsList::SimG4UserLimitPhysicsList(const std::string& aType, const std::string& aName,
                                                     const IInterface* aParent)
    : AlgTool(aType, aName, aParent) {
  declareInterface<ISimG4PhysicsList>(this);
  declareProperty("fullphysics", m_physicsListTool, "Handle for the full physics list tool");
}

SimG4UserLimitPhysicsList::~SimG4UserLimitPhysicsList() {}

StatusCode SimG4UserLimitPhysicsList::initialize() {
  if (AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_physicsListTool.retrieve();
  return StatusCode::SUCCESS;
}

StatusCode SimG4UserLimitPhysicsList::finalize() { return AlgTool::finalize(); }

G4VModularPhysicsList* SimG4UserLimitPhysicsList::physicsList() {
  // ownership passed to SimG4Svc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  G4VModularPhysicsList* physicsList = m_physicsListTool->physicsList();
  // Attach step limiter process
  physicsList->RegisterPhysics(new G4StepLimiterPhysics());
  return physicsList;
}
