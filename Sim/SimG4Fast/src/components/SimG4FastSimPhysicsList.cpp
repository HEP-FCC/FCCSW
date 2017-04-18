#include "SimG4FastSimPhysicsList.h"

// FCCSW
#include "SimG4Fast/FastSimPhysics.h"
#include "SimG4Interface/ISimG4PhysicsList.h"

// Geant4
#include "G4VModularPhysicsList.hh"

DECLARE_TOOL_FACTORY(SimG4FastSimPhysicsList)

SimG4FastSimPhysicsList::SimG4FastSimPhysicsList(const std::string& aType, const std::string& aName,
                                                 const IInterface* aParent)
    : AlgTool(aType, aName, aParent) {
  declareInterface<ISimG4PhysicsList>(this);
  declareProperty("fullphysics", m_physicsListTool, "Handle for the full physics list tool");
}

SimG4FastSimPhysicsList::~SimG4FastSimPhysicsList() {}

StatusCode SimG4FastSimPhysicsList::initialize() {
  if (AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_physicsListTool.retrieve();
  return StatusCode::SUCCESS;
}

StatusCode SimG4FastSimPhysicsList::finalize() { return AlgTool::finalize(); }

G4VModularPhysicsList* SimG4FastSimPhysicsList::physicsList() {
  // ownership passed to SimG4Svc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  G4VModularPhysicsList* physicsList = m_physicsListTool->physicsList();
  // Coupled transportation enables calculation of particle trajectory in envelopes with fast sim models attached
  G4PhysicsListHelper::GetPhysicsListHelper()->UseCoupledTransportation();
  // Attach Fast Simulation Process (will take over normal transportation if FastSimModel triggered
  physicsList->RegisterPhysics(new sim::FastSimPhysics);
  return physicsList;
}
