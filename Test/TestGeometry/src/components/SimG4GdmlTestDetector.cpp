#include "SimG4GdmlTestDetector.h"

// FCCSW
#include "TestGeometry/GdmlDetectorConstruction.h"

// Geant4
#include "G4VUserDetectorConstruction.hh"

DECLARE_TOOL_FACTORY(SimG4GdmlTestDetector)

SimG4GdmlTestDetector::SimG4GdmlTestDetector(const std::string& aType, const std::string& aName,
                                             const IInterface* aParent)
    : AlgTool(aType, aName, aParent) {
  declareInterface<ISimG4DetectorConstruction>(this);
}

SimG4GdmlTestDetector::~SimG4GdmlTestDetector() {}

StatusCode SimG4GdmlTestDetector::initialize() { return AlgTool::initialize(); }
StatusCode SimG4GdmlTestDetector::finalize() { return AlgTool::finalize(); }

G4VUserDetectorConstruction* SimG4GdmlTestDetector::detectorConstruction() {
  return new test::GdmlDetectorConstruction(m_gdmlFile);
}
