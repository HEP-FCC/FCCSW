#include "TestCellCounting.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"
#include "DetSegmentation/GridPhiEta.h"

// datamodel
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXY.h"
#include "DDSegmentation/CartesianGridXYZ.h"

// ROOT
#include "TGeoManager.h"

DECLARE_ALGORITHM_FACTORY(TestCellCounting)

TestCellCounting::TestCellCounting(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc) {}

StatusCode TestCellCounting::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }

  // get the total number of volumes in the geometry
  auto highestVol = gGeoManager->GetTopVolume();
  info() << "Number of modules whose name matches " << m_volumeMatchName << ": \t"
         << det::utils::countPlacedVolumes(highestVol, m_volumeMatchName) << endmsg;

  // get the ID of the volume for which the cells are counted
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  if (m_fieldNames.size() != m_fieldValues.size()) {
    error() << "Size of names and values is not the same" << endmsg;
    return StatusCode::FAILURE;
  }
  for (uint it = 0; it < m_fieldNames.size(); it++) {
    (*decoder)[m_fieldNames[it]] = m_fieldValues[it];
  }
  m_volumeId = decoder->getValue();

  // count the segmentation cells for the volume
  info() << "Counting cells for volume " << decoder->valueString() << " -> volume ID: " << m_volumeId << endmsg;
  auto segmentationXY = dynamic_cast<DD4hep::DDSegmentation::CartesianGridXY*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (segmentationXY == nullptr) {
    info() << "There is no Cartesian XY segmentation." << endmsg;
  } else {
    info() << "Number of segmentation cells in (x,y): " << det::utils::numberOfCells(m_volumeId, *segmentationXY)
           << endmsg;
  }
  auto segmentationXYZ = dynamic_cast<DD4hep::DDSegmentation::CartesianGridXYZ*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (segmentationXYZ == nullptr) {
    info() << "There is no Cartesian XYZ segmentation." << endmsg;
  } else {
    info() << "Number of segmentation cells in (x,y,z): " << det::utils::numberOfCells(m_volumeId, *segmentationXYZ)
           << endmsg;
  }
  auto segmentationRPhi = dynamic_cast<DD4hep::DDSegmentation::PolarGridRPhi*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (segmentationRPhi == nullptr) {
    info() << "There is no R-phi segmentation." << endmsg;
  } else {
    info() << "Number of segmentation cells in (r,phi): " << det::utils::numberOfCells(m_volumeId, *segmentationRPhi)
           << endmsg;
  }
  auto segmentationPhiEta = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (segmentationPhiEta == nullptr) {
    info() << "There is no phi-eta segmentation." << endmsg;
  } else {
    info() << "Number of segmentation cells in (phi,eta): "
           << det::utils::numberOfCells(m_volumeId, *segmentationPhiEta) << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode TestCellCounting::execute() { return StatusCode::SUCCESS; }

StatusCode TestCellCounting::finalize() { return GaudiAlgorithm::finalize(); }
