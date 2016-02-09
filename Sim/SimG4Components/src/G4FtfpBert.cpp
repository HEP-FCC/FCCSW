#include "G4FtfpBert.h"

// Geant4
#include "G4VModularPhysicsList.hh"
#include "FTFP_BERT.hh"

DECLARE_TOOL_FACTORY(G4FtfpBert)

G4FtfpBert::G4FtfpBert(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  AlgTool(aType, aName, aParent) {
  declareInterface<IG4PhysicsList>(this);
}

G4FtfpBert::~G4FtfpBert() {}

StatusCode G4FtfpBert::initialize() {
  return AlgTool::initialize();
}

StatusCode G4FtfpBert::finalize() {
  return AlgTool::finalize();
}

G4VModularPhysicsList* G4FtfpBert::getPhysicsList() {
   // ownership passed to G4SimSvc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  return new FTFP_BERT;
}

