#include "DDG4/Factories.h"
#include "DD4hep/LCDD.h"

#include "SimG4Full/GeantTrackerSD.h"

namespace DD4hep { namespace Simulation {

      // Factory method to create an instance of ExN04TrackerSD
    static G4VSensitiveDetector* createTrackerSD(const std::string& name,  DD4hep::Geometry::LCDD& lcdd)  {
       std::string trackerSDname = "/mydet/tracker";
       std::cout<<" Creating /mydet/tracker SD in G4"<<std::endl;
      return new GeantTrackerSD(trackerSDname);
    }
   }
}
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(DD4hepGeantTrackerSD,DD4hep::Simulation::createTrackerSD)
