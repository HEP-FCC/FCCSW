#ifndef RECTRACKER_TRKVOLUMEMANAGERSVC_H
#define RECTRACKER_TRKVOLUMEMANAGERSVC_H

#include "RecInterface/ITrkVolumeManagerSvc.h"

// Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h"

#include "DetInterface/ITrackingGeoSvc.h"



namespace Acts {
class Layer;
class TrackingVolume;
class Surface;
}


/** @class TrkGeoVolumeManagerSvc
 *
 * Vuilds a map to look up pointers to tracking surfaces for a given identifier.
 */


class TrkVolumeManagerSvc : public extends1<Service, ITrkVolumeManagerSvc> {
public:
  /// Constructor.
  explicit TrkVolumeManagerSvc(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Finalize.
  virtual StatusCode finalize();
  /// Destructor
  virtual ~TrkVolumeManagerSvc() {}

  void dumpTrackingLayer(std::shared_ptr<const Acts::Layer> layer);

  void dumpTrackingVolume(std::shared_ptr<const Acts::TrackingVolume> volume);

  const Acts::Surface* lookUpTrkSurface(const Identifier id);

private:
  SmartIF<ITrackingGeoSvc> m_geoSvc;
  std::map<const Identifier, const Acts::Surface*> m_surfaceManager;
};

#endif /* RECTRACKER_TRKVOLUMEMANAGERSVC_H */
