#include "CreateCaloCellPositions.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/Detector.h"

// EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

DECLARE_ALGORITHM_FACTORY(CreateCaloCellPositions)

CreateCaloCellPositions::CreateCaloCellPositions(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hits", m_hits, "Hit collection (input)");
  declareProperty("positionsECalBarrelTool", m_cellPositionsECalBarrelTool,
                  "Handle for tool to retrieve cell positions in ECal Barrel");
  declareProperty("positionsHCalBarrelTool", m_cellPositionsHCalBarrelTool,
                  "Handle for tool to retrieve cell positions in HCal Barrel and ext Barrel");
  declareProperty("positionsHCalExtBarrelTool", m_cellPositionsHCalExtBarrelTool,
                  "Handle for tool to retrieve cell positions in HCal Barrel and ext Barrel");
  declareProperty("positionsEMECTool", m_cellPositionsEMECTool, "Handle for tool to retrieve cell positions in EMEC");
  declareProperty("positionsHECTool", m_cellPositionsHECTool, "Handle for tool to retrieve cell positions in HEC");
  declareProperty("positionsEMFwdTool", m_cellPositionsEMFwdTool, "Handle for tool to retrieve cell positions EM Fwd");
  declareProperty("positionsHFwdTool", m_cellPositionsHFwdTool, "Handle for tool to retrieve cell positions Had Fwd");
  declareProperty("positionedHits", m_positionedHits, "Output cell positions collection");
}

StatusCode CreateCaloCellPositions::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloCellPositions::execute() {
  // Get the input hit collection
  const auto* hits = m_hits.get();
  debug() << "Input hit collection size: " << hits->size() << endmsg;
  // Initialize output collection
  auto edmPositionedHitCollection = m_positionedHits.createAndPut();

  for (const auto& hit : *hits) {
    auto cellId = hit.core().cellId;
    // identify calo system
    m_decoder->setValue(cellId);
    auto systemId = (*m_decoder)["system"].value();
    dd4hep::Position posCell;

    if (systemId == 5)  // ECAL BARREL system id
      posCell = m_cellPositionsECalBarrelTool->xyzPosition(cellId);
    else if (systemId == 8)  // HCAL BARREL system id
      posCell = m_cellPositionsHCalBarrelTool->xyzPosition(cellId);
    else if (systemId == 9)  // HCAL EXT BARREL system id
      posCell = m_cellPositionsHCalExtBarrelTool->xyzPosition(cellId);
    else if (systemId == 6)  // EMEC system id
      posCell = m_cellPositionsEMECTool->xyzPosition(cellId);
    else if (systemId == 7)  // HEC system id
      posCell = m_cellPositionsHECTool->xyzPosition(cellId);
    else if (systemId == 10)  // EMFWD system id
      posCell = m_cellPositionsEMFwdTool->xyzPosition(cellId);
    else if (systemId == 11)  // HFWD system id
      posCell = m_cellPositionsHFwdTool->xyzPosition(cellId);

    auto edmPos = fcc::Point();
    edmPos.x = posCell.x() / dd4hep::mm;
    edmPos.y = posCell.y() / dd4hep::mm;
    edmPos.z = posCell.z() / dd4hep::mm;

    auto positionedHit = edmPositionedHitCollection->create(edmPos, hit.core());

    // Debug information about cell position
    debug() << "Cell energy (GeV) : " << hit.core().energy << "\tcellID " << hit.core().cellId << endmsg;
    debug() << "Position of cell (mm) : \t" << posCell.x() / dd4hep::mm << "\t" << posCell.y() / dd4hep::mm << "\t"
            << posCell.z() / dd4hep::mm << endmsg;
  }

  debug() << "Output positions collection size: " << edmPositionedHitCollection->size() << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloCellPositions::finalize() { return GaudiAlgorithm::finalize(); }
