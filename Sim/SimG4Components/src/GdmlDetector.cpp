#include "GdmlDetector.h"

// FCCSW
#include "SimG4Common/GdmlDetectorConstruction.h"

// Geant4
#include "G4VUserDetectorConstruction.hh"

DECLARE_COMPONENT(GdmlDetector)

GdmlDetector::GdmlDetector(const std::string& aType, const std::string& aName, const IInterface* aParent) :
GaudiTool(aType, aName, aParent) {
  declareInterface<IG4DetectorConstruction>(this);
  declareProperty("gdml", m_gdmlFile);
}

GdmlDetector::~GdmlDetector() {}

StatusCode GdmlDetector::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

G4VUserDetectorConstruction* GdmlDetector::getDetectorConstruction() {
  return new sim::GdmlDetectorConstruction(m_gdmlFile);
}

