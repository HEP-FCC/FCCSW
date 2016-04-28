#include "DDG4/Factories.h"
#include "DD4hep/LCDD.h"

#include "DetSensitive/SimpleTrackerSD.h"
#include "DetSensitive/SimpleCalorimeterSD.h"

namespace DD4hep {
namespace Simulation {

// All G4VSensitiveDetector are deleted by ~G4SDManager called by ~G4RunManagerKernel
// Factory method to create an instance of SimpleTrackerSD
static G4VSensitiveDetector* create_simple_tracker_sd(
    const std::string& aDetectorName,
    DD4hep::Geometry::LCDD& aLcdd)  {
  std::cout<<"Creating an external SD of type <<SimpleTrackerSD>> with name "<<aDetectorName<<std::endl;
  std::string readoutName = aLcdd.sensitiveDetector(aDetectorName).readout().name();
  std::cout<<"readout name: "<<readoutName<<std::endl;
  return new det::SimpleTrackerSD(aDetectorName,
    readoutName,
    aLcdd.sensitiveDetector(aDetectorName).readout().segmentation());
}
// Factory method to create an instance of SimpleCalorimeterSD
static G4VSensitiveDetector* create_simple_calorimeter_sd(
    const std::string& aDetectorName,
    DD4hep::Geometry::LCDD& aLcdd)  {
  std::cout<<"Creating an external SD of type <<SimpleCalorimeterSD>> with name "<<aDetectorName<<std::endl;
  std::string readoutName = aLcdd.sensitiveDetector(aDetectorName).readout().name();
  std::cout<<"readout name: "<<readoutName<<std::endl;
  return new det::SimpleCalorimeterSD(aDetectorName,
    readoutName,
    aLcdd.sensitiveDetector(aDetectorName).readout().segmentation());
}
}
}
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(SimpleTrackerSD,DD4hep::Simulation::create_simple_tracker_sd)
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(SimpleCalorimeterSD,DD4hep::Simulation::create_simple_calorimeter_sd)
