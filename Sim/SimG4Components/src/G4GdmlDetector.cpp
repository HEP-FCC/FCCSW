#include "G4GdmlDetector.h"

// FCCSW
#include "SimG4Common/GdmlDetectorConstruction.h"

// Geant4
#include "G4VUserDetectorConstruction.hh"

DECLARE_TOOL_FACTORY(G4GdmlDetector)

G4GdmlDetector::G4GdmlDetector(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  AlgTool(aType, aName, aParent) {
  declareInterface<IG4DetectorConstruction>(this);
  declareProperty("gdml", m_gdmlFile);
}

G4GdmlDetector::~G4GdmlDetector() {}

StatusCode G4GdmlDetector::initialize() {
  return AlgTool::initialize();
}
StatusCode G4GdmlDetector::finalize() {
  return AlgTool::finalize();
}

G4VUserDetectorConstruction* G4GdmlDetector::getDetectorConstruction() {
  return new sim::GdmlDetectorConstruction(m_gdmlFile);
}

