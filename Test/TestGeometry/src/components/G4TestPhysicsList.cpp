#include "G4TestPhysicsList.h"
#include "TestGeometry/PhysicsList.h"

// Geant4
#include "G4VModularPhysicsList.hh"

DECLARE_TOOL_FACTORY(G4TestPhysicsList)

G4TestPhysicsList::G4TestPhysicsList(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  AlgTool(aType, aName, aParent) {
  declareInterface<IG4PhysicsList>(this);
}

G4TestPhysicsList::~G4TestPhysicsList() {}

StatusCode G4TestPhysicsList::initialize() {
  return AlgTool::initialize();
}

StatusCode G4TestPhysicsList::finalize() {
  return AlgTool::finalize();
}

G4VModularPhysicsList* G4TestPhysicsList::getPhysicsList() {
   // ownership passed to G4SimSvc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  return new test::PhysicsList;
}

