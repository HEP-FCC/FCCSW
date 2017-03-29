#include "SimG4GdmlDetector.h"

// FCCSW
#include "SimG4Common/GdmlDetectorConstruction.h"

// Geant4
#include "G4VUserDetectorConstruction.hh"

DECLARE_TOOL_FACTORY(SimG4GdmlDetector)

SimG4GdmlDetector::SimG4GdmlDetector(const std::string& aType, const std::string& aName, const IInterface* aParent)
    : AlgTool(aType, aName, aParent) {
  declareInterface<ISimG4DetectorConstruction>(this);
}

SimG4GdmlDetector::~SimG4GdmlDetector() {}

StatusCode SimG4GdmlDetector::initialize() { return AlgTool::initialize(); }
StatusCode SimG4GdmlDetector::finalize() { return AlgTool::finalize(); }

G4VUserDetectorConstruction* SimG4GdmlDetector::detectorConstruction() {
  return new sim::GdmlDetectorConstruction(m_gdmlFile);
}
