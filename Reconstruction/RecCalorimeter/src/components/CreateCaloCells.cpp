#include "CreateCaloCells.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// DD4hep
#include "DD4hep/LCDD.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

DECLARE_ALGORITHM_FACTORY(CreateCaloCells)

CreateCaloCells::CreateCaloCells(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareInput("hits", m_hits, "hits");
  declareOutput("cells", m_cells, "cells");

  declareProperty("calibTool", m_calibTool);
  declarePrivateTool(m_calibTool, "CalibrateCaloHitsTool");
  declareProperty("noiseTool", m_noiseTool);
  declarePrivateTool(m_noiseTool, "NoiseCaloCellsFlatTool");
  declareProperty("geometryTool", m_geoTool);
  declarePrivateTool(m_geoTool, "TubeLayerPhiEtaCaloTool");

  declareProperty("doCellCalibration", m_doCellCalibration = true);
  declareProperty("addCellNoise", m_addCellNoise = true);
  declareProperty("filterCellNoise", m_filterCellNoise = false);
}

StatusCode CreateCaloCells::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  info() << "CreateCaloCells initialized" << endmsg;
  info() << "do calibration : " << m_doCellCalibration << endmsg;
  info() << "add cell noise      : " << m_addCellNoise << endmsg;
  info() << "remove noise cells below threshold : " << m_filterCellNoise << endmsg;

  // Initialization of tools
  // Calibrate Geant4 energy to EM scale tool
  if (m_doCellCalibration) {
    if (!m_calibTool.retrieve()) {
      error() << "Unable to retrieve the calo cells calibration tool!!!" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  // Cell noise tool
  if (m_addCellNoise) {
    if (!m_noiseTool.retrieve()) {
      error() << "Unable to retrieve the calo cells noise tool!!!" << endmsg;
      return StatusCode::FAILURE;
    }
    // Geometry settings
    if (!m_geoTool.retrieve()) {
      error() << "Unable to retrieve the geometry tool!!!" << endmsg;
      return StatusCode::FAILURE;
    }
    // Prepare map of all existing cells in calorimeter to add noise to all
    StatusCode sc_prepareCells = m_geoTool->prepareEmptyCells(m_cellsMap);
    if (sc_prepareCells.isFailure()) {
      error() << "Unable to create empty cells!" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloCells::execute() {
  // Get the input collection with Geant4 hits
  const fcc::CaloHitCollection* hits = m_hits.get();
  debug() << "Input Hit collection size: " << hits->size() << endmsg;

  // 0. Clear all cells
  std::for_each(m_cellsMap.begin(), m_cellsMap.end(), [](std::pair<const uint64_t, double>& p) { p.second = 0; });

  // 1. Merge energy deposits into cells
  // If running with noise map already was prepared. Otherwise it is being created below
  for (const auto& hit : *hits) {
    m_cellsMap[hit.core().cellId] += hit.core().energy;
  }

  // 2. Calibrate simulation energy to EM scale
  if (m_doCellCalibration) {
    m_calibTool->calibrate(m_cellsMap);
  }

  // 3. Add noise to all cells
  if (m_addCellNoise) {
    m_noiseTool->addRandomCellNoise(m_cellsMap);
    if (m_filterCellNoise) {
      m_noiseTool->filterCellNoise(m_cellsMap);
    }
  }

  // 4. Copy information to CaloHitCollection
  fcc::CaloHitCollection* edmCellsCollection = new fcc::CaloHitCollection();
  for (const auto& cell : m_cellsMap) {
    if (cell.second != 0) {
      fcc::CaloHit newCell = edmCellsCollection->create();
      newCell.core().energy = cell.second;
      newCell.core().cellId = cell.first;
    }
  }

  // push the CaloHitCollection to event store
  m_cells.put(edmCellsCollection);
  debug() << "Output Cell collection size: " << edmCellsCollection->size() << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode CreateCaloCells::finalize() { return GaudiAlgorithm::finalize(); }
