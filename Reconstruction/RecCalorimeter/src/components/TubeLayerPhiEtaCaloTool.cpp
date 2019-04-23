#include "TubeLayerPhiEtaCaloTool.h"

// segm
#include "DD4hep/Detector.h"
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

#include "DDSegmentation/MultiSegmentation.h"

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
  const dd4hep::DDSegmentation::FCCSWGridPhiEta* segmentation = nullptr;
  const dd4hep::DDSegmentation::MultiSegmentation* segmentationMulti = nullptr;
  segmentation = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (segmentation == nullptr) {
    segmentationMulti = dynamic_cast<dd4hep::DDSegmentation::MultiSegmentation*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
    if (segmentationMulti == nullptr) {
      error() << "There is no phi-eta or multi- segmentation for the readout " << m_readoutName << " defined." << endmsg;
      return StatusCode::FAILURE;
    } else {
      // check if multisegmentation contains only phi-eta sub-segmentations
      const dd4hep::DDSegmentation::FCCSWGridPhiEta* subsegmentation = nullptr;
      for (const auto& subSegm: segmentationMulti->subSegmentations()) {
        subsegmentation = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(subSegm.segmentation);
        if (subsegmentation == nullptr) {
          error() << "At least one of the sub-segmentations in MultiSegmentation named " << m_readoutName << " is not a phi-eta grid." << endmsg;
          return StatusCode::FAILURE;
        } else {
          info() << "subsegmentation for " << segmentationMulti->discriminatorName() << " from " << subSegm.key_min << " to " << subSegm.key_max  << endmsg;
          info() << "size in eta " << subsegmentation->gridSizeEta() << " , bins in phi " << subsegmentation->phiBins()  << endmsg;
          info() << "offset in eta " << subsegmentation->offsetEta() << " , offset in phi " << subsegmentation->offsetPhi() << endmsg;
        }
      }
    }
  } else {
    info() << "FCCSWGridPhiEta: size in eta " << segmentation->gridSizeEta() << " , bins in phi " << segmentation->phiBins()
           << endmsg;
    info() << "FCCSWGridPhiEta: offset in eta " << segmentation->offsetEta() << " , offset in phi "
           << segmentation->offsetPhi() << endmsg;
  }
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
    dd4hep::DDSegmentation::VolumeID volumeID = 0;
    for (unsigned int it = 0; it < m_fieldNames.size(); it++) {
      decoder->set(volumeID, m_fieldNames[it], m_fieldValues[it]);
    }
    decoder->set(volumeID, m_activeFieldName, ilayer);
    decoder->set(volumeID, "eta", 0);
    decoder->set(volumeID, "phi", 0);

    if (segmentationMulti != nullptr) {
      segmentation = dynamic_cast<const dd4hep::DDSegmentation::FCCSWGridPhiEta*>(&segmentationMulti->subsegmentation(volumeID));
    }

    // Get number of segmentation cells within the active volume
    auto numCells = det::utils::numberOfCells(volumeID, *segmentation);
    debug() << "Segmentation cells  (Nphi, Neta, minEta): " << numCells << endmsg;
    // Loop over segmenation cells
    for (unsigned int iphi = 0; iphi < numCells[0]; iphi++) {
      for (unsigned int ieta = 0; ieta < numCells[1]; ieta++) {
        decoder->set(volumeID, "phi", iphi);
        decoder->set(volumeID, "eta", ieta + numCells[2]); // start from the minimum existing eta cell in this layer
        dd4hep::DDSegmentation::CellID cellId = volumeID;
        aCells.insert(std::pair<dd4hep::DDSegmentation::CellID, double>(cellId, 0));
      }
    }
  }
  return StatusCode::SUCCESS;
}
