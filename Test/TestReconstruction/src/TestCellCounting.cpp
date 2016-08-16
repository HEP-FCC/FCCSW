#include "TestCellCounting.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXYZ.h"

#include "TGeoManager.h"

DECLARE_ALGORITHM_FACTORY(TestCellCounting)

TestCellCounting::TestCellCounting(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("readoutName", m_readoutName);
  declareProperty("fieldNames", m_fieldNames);
  declareProperty("fieldValues", m_fieldValues);
  declareProperty("volumeMatchName", m_volumeMatchName, "Silicon");
}

TestCellCounting::~TestCellCounting() {}

StatusCode TestCellCounting::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }

  auto highestVol = gGeoManager->GetTopVolume();
  info() << "Number of modules whose name matches " << m_volumeMatchName << ": \t" <<det::utils::countPlacedVolumes(highestVol, m_volumeMatchName) <<endmsg;

  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  if(m_fieldNames.size() != m_fieldValues.size()) {
    error() << "Size of names and values is not the same" << endmsg;
    return StatusCode::FAILURE;
  }
  for(uint it=0; it<m_fieldNames.size(); it++) {
    (*decoder)[m_fieldNames[it]] = m_fieldValues[it];
  }
  m_volumeId = decoder->getValue();
  auto segmentation = dynamic_cast<DD4hep::DDSegmentation::CartesianGridXYZ*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if(segmentation == nullptr) {
    info()<<"There is no segmentation."<<endmsg;
  } else {
    info()<<"Number of segmentation cells in (x,y,z): "<<det::utils::numberOfCellsInCartesian(m_volumeId, *segmentation)<<endmsg;

  }
  return StatusCode::SUCCESS;
}

StatusCode TestCellCounting::execute() {
  return StatusCode::SUCCESS;
}

StatusCode TestCellCounting::finalize() {
  return GaudiAlgorithm::finalize();
}
