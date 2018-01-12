#include "CreateCellPositions.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/LCDD.h"

// EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

DECLARE_ALGORITHM_FACTORY(CreateCellPositions)

CreateCellPositions::CreateCellPositions(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hits", m_hits, "Hit collection (input)");
  declareProperty("positionsTool", m_cellPositionsTool, "Handle for tool to retrieve cell positions");
  declareProperty("positionedHits", m_positionedHits, "Output cell positions collection");
}

StatusCode CreateCellPositions::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  if (!m_cellPositionsTool.retrieve()) {
    error() << "Unable to retrieve the cell positions tool!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode CreateCellPositions::execute() {
  // Get the input hit collection
  const auto* hits = m_hits.get();
  debug() << "Input hit collection size: " << hits->size() << endmsg;
  // Initialize output collection
  auto edmPositionedHitCollection = m_positionedHits.createAndPut();

  m_cellPositionsTool->getPositions(*hits, *edmPositionedHitCollection);

  debug() << "Output positions collection size: " << edmPositionedHitCollection->size() << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CreateCellPositions::finalize() { return GaudiAlgorithm::finalize(); }
