#include "DDG4/Factories.h"
#include "DD4hep/LCDD.h"

#include "SimG4Full/GeantTrackerSD.h"

namespace sim {
namespace detectors {
// Factory method to create an instance of GeantTrackerSD
static G4VSensitiveDetector* createTrackerSD(const std::string& name,  DD4hep::Geometry::LCDD& lcdd) {
  std::string trackerSDname = "/mydet/tracker/"+name;
  /// tmp debug
  std::cout<<" SENSITIVE VOLUME: "<<trackerSDname<<std::endl;
  std::cout<<" SENSITIVE VOLUME hits : "<<lcdd.sensitiveDetector(name).hitsCollection()<<std::endl;
  return new GeantTrackerSD(trackerSDname,lcdd.sensitiveDetector(name).hitsCollection());
}
}
}
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(DD4hepGeantTrackerSD,sim::detectors::createTrackerSD)
