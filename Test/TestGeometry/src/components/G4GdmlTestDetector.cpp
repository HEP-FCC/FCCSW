#include "G4GdmlTestDetector.h"

// FCCSW
#include "TestGeometry/GdmlDetectorConstruction.h"

// Geant4
#include "G4VUserDetectorConstruction.hh"

DECLARE_TOOL_FACTORY(G4GdmlTestDetector)

G4GdmlTestDetector::G4GdmlTestDetector(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  AlgTool(aType, aName, aParent) {
  declareInterface<IG4DetectorConstruction>(this);
  declareProperty("gdml", m_gdmlFile);
}

G4GdmlTestDetector::~G4GdmlTestDetector() {}

StatusCode G4GdmlTestDetector::initialize() {
  return AlgTool::initialize();
}
StatusCode G4GdmlTestDetector::finalize() {
  return AlgTool::finalize();
}

G4VUserDetectorConstruction* G4GdmlTestDetector::getDetectorConstruction() {
  return new test::GdmlDetectorConstruction(m_gdmlFile);
}

