#include "TestDumpDetElements.h"

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

DECLARE_ALGORITHM_FACTORY(TestDumpDetElements)

TestDumpDetElements::TestDumpDetElements(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc) {}

StatusCode TestDumpDetElements::initialize() {
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

  return StatusCode::SUCCESS;
}

StatusCode TestDumpDetElements::execute() { return StatusCode::SUCCESS; }

StatusCode TestDumpDetElements::finalize() { return GaudiAlgorithm::finalize(); }
