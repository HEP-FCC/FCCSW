#include "DDG4/Factories.h"
#include "DD4hep/LCDD.h"

#include "SimpleTrackerSD.h"

namespace DD4hep {
namespace Simulation {

// Factory method to create an instance of SimpleTrackerSD
static G4VSensitiveDetector* create_example_tracker_sd(
    const std::string& aDetectorName,
    DD4hep::Geometry::LCDD& aLcdd)  {
  std::cout<<"Creating an external SD of type <<SimpleTrackerSD>> with name "<<aDetectorName<<std::endl;
  auto det = aLcdd.volumeManager().detector().children().at(aDetectorName);
  std::string readoutName = aLcdd.sensitiveDetector(aDetectorName).readout().name();
  std::cout<<"readout name: "<<readoutName<<std::endl;
  return new det::SimpleTrackerSD(aDetectorName,
    readoutName,
    aLcdd.sensitiveDetector(aDetectorName).readout().segmentation());
}
}
}
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(SimpleTrackerSD,DD4hep::Simulation::create_example_tracker_sd)
