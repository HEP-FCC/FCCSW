#include "G4DD4hepDetector.h"

// FCCSW
#include "DetDesInterfaces/IGeoSvc.h"

// Geant4
#include "G4VUserDetectorConstruction.hh"

DECLARE_TOOL_FACTORY(G4DD4hepDetector)

G4DD4hepDetector::G4DD4hepDetector(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  GaudiTool(aType, aName, aParent) {
  declareInterface<IG4DetectorConstruction>(this);
}

G4DD4hepDetector::~G4DD4hepDetector() {}

StatusCode G4DD4hepDetector::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error()<<"Unable to locate Geometry Service"<<endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode G4DD4hepDetector::finalize() {
  return GaudiTool::finalize();
}

G4VUserDetectorConstruction* G4DD4hepDetector::getDetectorConstruction() {
  return m_geoSvc->getGeant4Geo();
}
