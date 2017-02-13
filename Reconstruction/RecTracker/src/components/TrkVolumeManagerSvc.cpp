#include "TrkVolumeManagerSvc.h"

#include "TGeoManager.h"


#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Detector/TrackingVolume.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"
#include "ACTS/Examples/BuildGenericDetector.hpp"
#include "ACTS/Utilities/Identifier.hpp"

DECLARE_SERVICE_FACTORY(TrkVolumeManagerSvc)

TrkVolumeManagerSvc::TrkVolumeManagerSvc(const std::string& aName, ISvcLocator* aSL) : base_class(aName, aSL) {
}

void TrkVolumeManagerSvc::dumpTrackingVolume(const Acts::TrackingVolume* vol) {
  info() << "Dumping TrackingVolume" << vol->volumeName() << endmsg;
  auto volVec = vol->confinedVolumes();
  if (nullptr != volVec) {
    auto objVec = volVec->arrayObjects();
    for (auto o : objVec) {
      dumpTrackingVolume(o.get());
    }
  }
  auto layArr = vol->confinedLayers();
  if (nullptr != layArr) {
    auto layVec = layArr->arrayObjects();
    info() << "\t containing " << layVec.size() << " Layers" << endmsg;
    for (auto l : layVec) {
      dumpTrackingLayer(l.get());
    }
  }
}

void TrkVolumeManagerSvc::dumpTrackingLayer(const Acts::Layer* layer) {
  auto surfArr = layer->surfaceArray();

  if (nullptr != surfArr) {
    auto surfVec = surfArr->arrayObjects();
    info() << "\t\t layer contains " << surfVec.size() << " TrackingSurfaces" << endmsg;
    for (auto s : surfVec) {
      m_surfaceManager.insert(std::make_pair(s->associatedIdentifier(), s));
    }
  }
}

StatusCode TrkVolumeManagerSvc::initialize() {
  m_geoSvc = service("TrackingGeoSvc");
  if (nullptr == m_geoSvc) {
    error() << "Unable to locate Tracking Geometry Service" << endmsg;
    return StatusCode::FAILURE;
  }
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  std::shared_ptr<const Acts::TrackingGeometry> tgeo = m_geoSvc->trackingGeometry();
  auto highestVol = tgeo->highestTrackingVolume();
  info() << "Passing TrackingVolume" << highestVol->volumeName() << endmsg;
  // recurse through tracking geometry
  dumpTrackingVolume(highestVol);
  return StatusCode::SUCCESS;
}


const Acts::Surface* TrkVolumeManagerSvc::lookUpTrkSurface(const Identifier id) { return m_surfaceManager[id];}


StatusCode TrkVolumeManagerSvc::finalize() { return Service::finalize(); }
