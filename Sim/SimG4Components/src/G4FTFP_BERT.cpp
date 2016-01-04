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
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

G4VModularPhysicsList* G4FTFP_BERT::getPhysicsList() {
   // ownership passed to GeantSvc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  return new FTFP_BERT;
}

