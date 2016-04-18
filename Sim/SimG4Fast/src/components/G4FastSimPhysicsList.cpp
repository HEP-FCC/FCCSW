#include "G4FastSimPhysicsList.h"

// FCCSW
#include "SimG4Fast/FastSimPhysics.h"
#include "SimG4Interface/IG4PhysicsList.h"

// Geant4
#include "G4VModularPhysicsList.hh"

DECLARE_TOOL_FACTORY(G4FastSimPhysicsList)

G4FastSimPhysicsList::G4FastSimPhysicsList(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  AlgTool(aType, aName, aParent) {
  declareInterface<IG4PhysicsList>(this);
  declareProperty("fullphysics", m_physicsListTool);
  declarePrivateTool(m_physicsListTool);
}

G4FastSimPhysicsList::~G4FastSimPhysicsList() {}

StatusCode G4FastSimPhysicsList::initialize() {
  if(AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_physicsListTool.retrieve();
  return StatusCode::SUCCESS;
}

StatusCode G4FastSimPhysicsList::finalize() {
  return AlgTool::finalize();
}

G4VModularPhysicsList* G4FastSimPhysicsList::getPhysicsList() {
  // ownership passed to G4SimSvc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  G4VModularPhysicsList* physicsList = m_physicsListTool->getPhysicsList();
  // Coupled transportation enables calculation of particle trajectory in envelopes with fast sim models attached
  G4PhysicsListHelper::GetPhysicsListHelper()->UseCoupledTransportation();
  // Attach Fast Simulation Process (will take over normal transportation if FastSimModel triggered
  physicsList->RegisterPhysics(new sim::FastSimPhysics);
  return physicsList;
}

