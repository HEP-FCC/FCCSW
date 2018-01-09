#include "CreateEmptyCaloCellsCollection.h"

// datamodel
#include "datamodel/CaloHitCollection.h"

DECLARE_ALGORITHM_FACTORY(CreateEmptyCaloCellsCollection)

CreateEmptyCaloCellsCollection::CreateEmptyCaloCellsCollection(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("cells", m_caloCells, "Empty calorimeter cells output)");
}

StatusCode CreateEmptyCaloCellsCollection::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  info() << "Create dummy cells collection initialized" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CreateEmptyCaloCellsCollection::execute() { 
  //create a new empty calo cells collection
  m_caloCells.createAndPut();
  return StatusCode::SUCCESS; 
}

StatusCode CreateEmptyCaloCellsCollection::finalize() { return GaudiAlgorithm::finalize(); }
