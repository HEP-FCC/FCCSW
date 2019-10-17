#include "SimG4DD4hepDetector.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// Geant4
#include "G4VUserDetectorConstruction.hh"

DECLARE_COMPONENT(SimG4DD4hepDetector)

SimG4DD4hepDetector::SimG4DD4hepDetector(const std::string& aType, const std::string& aName, const IInterface* aParent)
    : GaudiTool(aType, aName, aParent), m_geoSvc("GeoSvc", aName) {
  declareInterface<ISimG4DetectorConstruction>(this);
  declareProperty("GeoSvc", m_geoSvc);
}

SimG4DD4hepDetector::~SimG4DD4hepDetector() {}

StatusCode SimG4DD4hepDetector::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4DD4hepDetector::finalize() { return GaudiTool::finalize(); }

G4VUserDetectorConstruction* SimG4DD4hepDetector::detectorConstruction() { return m_geoSvc->getGeant4Geo(); }
