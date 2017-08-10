#include "CreateDummyCellsCollection.h"

// our EDM
#include "datamodel/CaloHitCollection.h"

DECLARE_ALGORITHM_FACTORY(CreateDummyCellsCollection)

CreateDummyCellsCollection::CreateDummyCellsCollection(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("cells", m_caloCells, "Dummy calorimeter cells output)");
}

StatusCode CreateDummyCellsCollection::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  info() << "Create dummy cells collection initialized" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CreateDummyCellsCollection::execute() { 
  //create a new calo cells collection
  m_caloCells.createAndPut();
  return StatusCode::SUCCESS; 
}

StatusCode CreateDummyCellsCollection::finalize() { return GaudiAlgorithm::finalize(); }
