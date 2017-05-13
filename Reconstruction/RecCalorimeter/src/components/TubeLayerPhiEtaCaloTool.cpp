#include "TubeLayerPhiEtaCaloTool.h"

// segm
#include "DD4hep/LCDD.h"
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

DECLARE_TOOL_FACTORY(TubeLayerPhiEtaCaloTool)

TubeLayerPhiEtaCaloTool::TubeLayerPhiEtaCaloTool(const std::string& type, const std::string& name,
                                                 const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ICalorimeterTool>(this);
}

StatusCode TubeLayerPhiEtaCaloTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_readoutName != "") {
    // Check if readouts exist
    info() << "Readout: " << m_readoutName << endmsg;
    if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
      error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
      return StatusCode::FAILURE;
    }
  }
  return sc;
}

StatusCode TubeLayerPhiEtaCaloTool::finalize() { return GaudiTool::finalize(); }

StatusCode TubeLayerPhiEtaCaloTool::prepareEmptyCells(std::unordered_map<uint64_t, double>& aCells) {
  // Get the total number of active volumes in the geometry
  auto highestVol = gGeoManager->GetTopVolume();
  unsigned int numLayers;
  if (!m_activeVolumesNumber) {
    numLayers = det::utils::countPlacedVolumes(highestVol, m_activeVolumeName);
  } else {
    // used when MergeLayers tool is used. To be removed once MergeLayer gets replaced by RedoSegmentation.
    numLayers = m_activeVolumesNumber;
  }
  info() << "Number of active layers " << numLayers << endmsg;

  // get PhiEta segmentation
  DD4hep::DDSegmentation::GridPhiEta* segmentation;
  segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (segmentation == nullptr) {
    error() << "There is no phi-eta segmentation!!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  info() << "GridPhiEta: size in eta " << segmentation->gridSizeEta() << " , bins in phi " << segmentation->phiBins()
         << endmsg;
  info() << "GridPhiEta: offset in eta " << segmentation->offsetEta() << " , offset in phi "
         << segmentation->offsetPhi() << endmsg;

  // Take readout bitfield decoder from GeoSvc
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  if (m_fieldNames.size() != m_fieldValues.size()) {
    error() << "Volume readout field descriptors (names and values) have different size." << endmsg;
    return StatusCode::FAILURE;
  }

  // Loop over all cells in the calorimeter and retrieve existing cellIDs
  // Loop over active layers
  for (unsigned int ilayer = 0; ilayer < numLayers; ilayer++) {
    // Get VolumeID
    for (unsigned int it = 0; it < m_fieldNames.size(); it++) {
      (*decoder)[m_fieldNames[it]] = m_fieldValues[it];
    }
    (*decoder)[m_activeFieldName] = ilayer;
    uint64_t volumeId = decoder->getValue();

    // Get number of segmentation cells within the active volume
    auto numCells = det::utils::numberOfCells(volumeId, *segmentation);
    debug() << "Number of segmentation cells in (phi,eta): " << numCells << endmsg;
    // Loop over segmenation cells
    for (unsigned int iphi = 0; iphi < numCells[0]; iphi++) {
      for (unsigned int ieta = 0; ieta < numCells[1]; ieta++) {
        (*decoder)["phi"] = iphi;
        (*decoder)["eta"] = ieta;
        uint64_t cellId = decoder->getValue();
        aCells.insert(std::pair<uint64_t, double>(cellId, 0));
      }
    }
  }
  return StatusCode::SUCCESS;
}
