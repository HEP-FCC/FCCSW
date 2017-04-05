#include "SimG4TestPhysicsList.h"
#include "TestGeometry/TestPhysicsList.h"

// Geant4
#include "G4VModularPhysicsList.hh"

DECLARE_TOOL_FACTORY(SimG4TestPhysicsList)

SimG4TestPhysicsList::SimG4TestPhysicsList(const std::string& aType, const std::string& aName,
                                           const IInterface* aParent)
    : AlgTool(aType, aName, aParent) {
  declareInterface<ISimG4PhysicsList>(this);
}

SimG4TestPhysicsList::~SimG4TestPhysicsList() {}

StatusCode SimG4TestPhysicsList::initialize() { return AlgTool::initialize(); }

StatusCode SimG4TestPhysicsList::finalize() { return AlgTool::finalize(); }

G4VModularPhysicsList* SimG4TestPhysicsList::physicsList() {
  // ownership passed to SimG4Svc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  return new test::TestPhysicsList;
}
