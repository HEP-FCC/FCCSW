#include "NoSegmentationCaloTool.h"

// segm
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"
#include "DD4hep/LCDD.h"

DECLARE_TOOL_FACTORY(NoSegmentationCaloTool)

NoSegmentationCaloTool::NoSegmentationCaloTool(const std::string& type,const std::string& name, const IInterface* parent)
: GaudiTool(type, name, parent) {
  declareInterface<ICalorimeterTool>(this);
  declareProperty("readoutName", m_readoutName = "ECalHitsPhiEta");
  declareProperty("activeVolumeName", m_activeVolumeName="LAr_sensitive");
  declareProperty("activeFieldName", m_activeFieldName="active_layer");
  declareProperty("activeVolumesNumber", m_activeVolumesNumber = 0);
  declareProperty("fieldNames", m_fieldNames);
  declareProperty("fieldValues", m_fieldValues);
}

NoSegmentationCaloTool::~NoSegmentationCaloTool() {}

StatusCode NoSegmentationCaloTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
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
  return sc;
}

StatusCode NoSegmentationCaloTool::finalize() {
  return GaudiTool::finalize();
}

StatusCode NoSegmentationCaloTool::prepareEmptyCells(std::unordered_map<uint64_t, double>& aCells) {
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
    aCells.insert(std::pair<uint64_t,double>(volumeId,0));
  }
  return StatusCode::SUCCESS;
}
