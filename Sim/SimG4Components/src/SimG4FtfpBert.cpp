#include "SimG4FtfpBert.h"

// Geant4
#include "FTFP_BERT.hh"
#include "G4VModularPhysicsList.hh"

DECLARE_TOOL_FACTORY(SimG4FtfpBert)

SimG4FtfpBert::SimG4FtfpBert(const std::string& aType, const std::string& aName, const IInterface* aParent)
    : AlgTool(aType, aName, aParent) {
  declareInterface<ISimG4PhysicsList>(this);
}

SimG4FtfpBert::~SimG4FtfpBert() {}

StatusCode SimG4FtfpBert::initialize() { return AlgTool::initialize(); }

StatusCode SimG4FtfpBert::finalize() { return AlgTool::finalize(); }

G4VModularPhysicsList* SimG4FtfpBert::physicsList() {
  // ownership passed to SimG4Svc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  return new FTFP_BERT;
}
