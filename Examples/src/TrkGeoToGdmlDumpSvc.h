#ifndef EXAMPLES_TRKGEOTOGDMLDUMPSVC_H
#define EXAMPLES_TRKGEOTOGDMLDUMPSVC_H

#include "ITestDD4hepSvc.h"

// Gaudi
#include "GaudiKernel/Service.h"

#include "DetInterface/ITrackingGeoSvc.h"

namespace Acts {
  class Layer;
  class TrackingVolume;
}
class TGeoVolume;

class TrkGeoToGdmlDumpSvc: public extends1<Service, ITestDD4hepSvc> {
public:
   /// Constructor.
   explicit TrkGeoToGdmlDumpSvc(const std::string& name, ISvcLocator* svcLoc);
   /// Initialize.
   virtual StatusCode initialize();
   /// Finalize.
   virtual StatusCode finalize();
   /// Destructor
   virtual ~TrkGeoToGdmlDumpSvc() {}

  void dumpTrackingLayer(const Acts::Layer* layer, TGeoVolume* top);

  void dumpTrackingVolume(const Acts::TrackingVolume* volume, TGeoVolume* top);
private:
   /// Name of the GDML output file
   std::string m_gdmlFileName;
   SmartIF<ITrackingGeoSvc> m_geoSvc;
};

#endif /* EXAMPLES_TRKGEOTOGDMLDUMPSVC_H */
