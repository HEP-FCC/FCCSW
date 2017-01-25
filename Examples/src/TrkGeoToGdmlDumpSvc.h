#ifndef EXAMPLES_TRKGEOTOGDMLDUMPSVC_H
#define EXAMPLES_TRKGEOTOGDMLDUMPSVC_H

#include "DetInterface/ITrkGeoDumpSvc.h"

// Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h"

#include "DetInterface/ITrackingGeoSvc.h"



namespace Acts {
class Layer;
class TrackingVolume;
class Surface;
}
class TGeoVolume;


/** @class TrkGeoToGdmlDumpSvc
 *
 * Converting and saving the tracking geometry is possible with this service.
 * In addition, it builds a map to look up pointers to tracking surfaces for a given identifier.
 */


class TrkGeoToGdmlDumpSvc : public extends1<Service, ITrkGeoDumpSvc> {
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

  const Acts::Surface* lookUpTrkSurface(const Identifier id);

private:
  /// Name of the GDML output file
  std::string m_gdmlFileName;
  SmartIF<ITrackingGeoSvc> m_geoSvc;
  std::map<const Identifier, const Acts::Surface*> m_surfaceManager;
};

#endif /* EXAMPLES_TRKGEOTOGDMLDUMPSVC_H */
