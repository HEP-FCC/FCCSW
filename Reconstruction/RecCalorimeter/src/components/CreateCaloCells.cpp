#include "CreateCaloCells.h"

//FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// DD4hep
#include "DD4hep/LCDD.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

DECLARE_ALGORITHM_FACTORY(CreateCaloCells)

CreateCaloCells::CreateCaloCells(const std::string& name, ISvcLocator* svcLoc)
: GaudiAlgorithm(name, svcLoc) {
  declareInput("hits", m_hits, "hits");
  declareOutput("cells", m_cells, "cells");

  declareProperty("calibTool", m_calibTool);
  declarePrivateTool(m_calibTool, "CalibrateCaloHitsTool");
  declareProperty("noiseTool", m_noiseTool);
  declarePrivateTool(m_noiseTool, "NoiseCaloCellsFlatTool");

  declareProperty("doCellCalibration", m_doCellCalibration = true);
  declareProperty("addCellNoise", m_addCellNoise = true);
  declareProperty("filterCellNoise", m_filterCellNoise = false);

  // Readout details
  declareProperty("readoutName", m_readoutName = "ECalHitsPhiEta");
  declareProperty("activeVolumeName", m_activeVolumeName="LAr_sensitive");
  declareProperty("activeFieldName", m_activeFieldName="active_layer");
  declareProperty("activeVolumesNumber", m_activeVolumesNumber = 0);
  declareProperty("fieldNames", m_fieldNames);
  declareProperty("fieldValues", m_fieldValues);
  declareProperty("useVolumeIdOnly", m_useVolumeIdOnly = false);
}

StatusCode CreateCaloCells::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  info() << "CreateCaloCells initialized" << endmsg;
  info() << "doCellCalibration : " << m_doCellCalibration << endmsg;
  info() << "addCellNoise      : " << m_addCellNoise << endmsg;
  info() << "filterCellNoise   : " << m_filterCellNoise << endmsg;

  // Initialization of tools
  // Calibrate Geant4 energy to EM scale tool
  if (m_doCellCalibration) {
    if (!m_calibTool.retrieve()) {
      error()<<"Unable to retrieve the calo cells calibration tool!!!"<<endmsg;
      return StatusCode::FAILURE;
    }
  }
  // Cell noise tool
  if (m_addCellNoise) {
    if (!m_noiseTool.retrieve()) {
      error()<<"Unable to retrieve the calo cells noise tool!!!"<<endmsg;
      return StatusCode::FAILURE;
    }
  }
  // Prepare map of all existing cells in calorimeter to add noise to all, also needed for clustering
  StatusCode sc_prepareCells = prepareEmptyCells();
  if (sc_prepareCells.isFailure()) {
    error()<<"Unable to create empty cells!"<<endmsg;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloCells::execute() {
  //Get the input collection with Geant4 hits
  const fcc::CaloHitCollection* hits = m_hits.get();
  debug() << "Input Hit collection size: " << hits->size() << endmsg;

  // 0. Clear all cells
  std::for_each( m_cellsMap.begin(), m_cellsMap.end(), [](std::pair<const uint64_t , double>& p) {p.second = 0;} );

  // 1. Merge energy deposits into cells
  // If running with noise map already was prepared. Otherwise it is being created below
  for(const auto& hit: *hits) {
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
    if( cell.second != 0 ) {
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

StatusCode CreateCaloCells::finalize() {
  return GaudiAlgorithm::finalize();
}

StatusCode CreateCaloCells::prepareEmptyCells() {
  // Get the geometry to know how many cells exist
  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // Check if readouts exist
  info()<<"Readout: "<<m_readoutName<< endmsg;
  if(m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_readoutName<<">> does not exist."<<endmsg;
    return StatusCode::FAILURE;
  }
  // Get the total number of active volumes in the geometry
  auto highestVol = gGeoManager->GetTopVolume();
  unsigned int numLayers;
  if( !m_activeVolumesNumber ) {
    numLayers = det::utils::countPlacedVolumes(highestVol, m_activeVolumeName);
  } else {
    // used when MergeLayers tool is used. To be removed once MergeLayer gets replaced by RedoSegmentation.
    numLayers = m_activeVolumesNumber;
  }
  info() << "Number of active layers " << numLayers << endmsg;

  // if using segmentation: get PhiEta segmentation
  DD4hep::DDSegmentation::GridPhiEta* segmentation;
  if (!m_useVolumeIdOnly) {
    segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
    if(segmentation == nullptr) {
      error()<<"There is no phi-eta segmentation!!!!"<<endmsg;
      return StatusCode::FAILURE;
    }
  }

  // Take readout bitfield decoder from GeoSvc
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  if(m_fieldNames.size() != m_fieldValues.size()) {
    error() << "Volume readout field descriptors (names and values) have different size." << endmsg;
    return StatusCode::FAILURE;
  }

  // Loop over all cells in the calorimeter and retrieve existing cellIDs
  // Loop over active layers
  for (unsigned int ilayer = 0; ilayer<numLayers; ilayer++) {
    //Get VolumeID
    for(uint it=0; it<m_fieldNames.size(); it++) {
      (*decoder)[m_fieldNames[it]] = m_fieldValues[it];
    }
    (*decoder)[m_activeFieldName] = ilayer;
    uint64_t volumeId = decoder->getValue();

    if (!m_useVolumeIdOnly) {
      // Get number of segmentation cells within the active volume
      auto numCells = det::utils::numberOfCells(volumeId, *segmentation);
      debug() << "Number of segmentation cells in (phi,eta): " << numCells << endmsg;
      // Loop over segmenation cells
      for (int iphi = -floor(numCells[0]*0.5); iphi<numCells[0]*0.5; iphi++) {
        for (int ieta = -floor(numCells[1]*0.5); ieta<numCells[1]*0.5; ieta++) {
          (*decoder)["phi"] = iphi;
          (*decoder)["eta"] = ieta;
          uint64_t cellId = decoder->getValue();
          m_cellsMap.insert(std::pair<uint64_t,double>(cellId,0));
        }
      }
    }
    else {
      m_cellsMap.insert(std::pair<uint64_t,double>(volumeId,0));
    }
  }
  return StatusCode::SUCCESS;
}
