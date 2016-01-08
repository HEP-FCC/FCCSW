#include "G4FTFP_BERT.h"

// Geant4
#include "G4VModularPhysicsList.hh"
#include "FTFP_BERT.hh"

DECLARE_COMPONENT(G4FTFP_BERT)

G4FTFP_BERT::G4FTFP_BERT(const std::string& aType, const std::string& aName, const IInterface* aParent) :
GaudiTool(aType, aName, aParent) {
  declareInterface<IG4PhysicsList>(this);
}

G4FTFP_BERT::~G4FTFP_BERT() {}

StatusCode G4FTFP_BERT::initialize() {
  return GaudiTool::initialize();
}

StatusCode G4FTFP_BERT::finalize() {
  return GaudiTool::finalize();
}

G4VModularPhysicsList* G4FTFP_BERT::getPhysicsList() {
   // ownership passed to G4SimSvc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  return new FTFP_BERT;
}

