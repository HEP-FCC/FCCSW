#include "DummyTrkGeoToGdmlDumpSvc.h"

#include "TGeoManager.h"

#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Detector/TrackingVolume.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Plugins/DD4hepPlugins/DD4hepCylinderGeometryBuilder.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"
#include "ACTS/Examples/BuildGenericDetector.hpp"

DECLARE_SERVICE_FACTORY(TrkGeoToGdmlDumpSvc)

TrkGeoToGdmlDumpSvc::TrkGeoToGdmlDumpSvc(const std::string& aName, ISvcLocator* aSL) : base_class(aName, aSL) {
  declareProperty("gdml", m_gdmlFileName = "TrkDetector.gdml");
}

void TrkGeoToGdmlDumpSvc::dumpTrackingVolume(const Acts::TrackingVolume* vol, TGeoVolume* top) {
  info() << "Dumping TrackingVolume" << vol->volumeName() << endmsg;
  auto volVec = vol->confinedVolumes();
  if (nullptr != volVec) {
    auto objVec = volVec->arrayObjects();
    for (auto o : objVec) {
      dumpTrackingVolume(o.get(), top);
    }
  }
  auto layArr = vol->confinedLayers();
  if (nullptr != layArr) {
    auto layVec = layArr->arrayObjects();
    info() << "\t containing " << layVec.size() << " Layers" << endmsg;
    for (auto l : layVec) {
      dumpTrackingLayer(l.get(), top);
    }
  }
}

void TrkGeoToGdmlDumpSvc::dumpTrackingLayer(const Acts::Layer* layer, TGeoVolume* top) {
  auto surfArr = layer->surfaceArray();

  // temporary TGeoVolume that represents one module
  /// @todo: get dimensions from surface
  int module_counter = 0;
  TGeoMaterial* matAl = new TGeoMaterial("Aluminium", 26, 13, 3);
  TGeoMedium* Al = new TGeoMedium("Aluminium", 2, matAl);
  TGeoVolume* module = gGeoManager->MakeBox("module", Al, 1, 1, 1);

  if (nullptr != surfArr) {
    auto surfVec = surfArr->arrayObjects();
    info() << "\t\t layer contains " << surfVec.size() << " TrackingSurfaces" << endmsg;
    for (auto s : surfVec) {
      auto transform = s->transform().translation();
      top->AddNode(module, module_counter, new TGeoTranslation(transform[0], transform[1], transform[2]));
    }
  }
}

StatusCode TrkGeoToGdmlDumpSvc::initialize() {
  m_geoSvc = service("TrackingGeometryService");
  if (!m_geoSvc) {
    error() << "Unable to locate Tracking Geometry Service" << endmsg;
    return StatusCode::FAILURE;
  }
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  std::shared_ptr<const Acts::TrackingGeometry> tgeo = m_geoSvc->trackingGeometry();
  auto highestVol = tgeo->highestTrackingVolume();
  info() << "Dumping TrackingVolume" << highestVol->volumeName() << endmsg;
  // temporary TGeoVolume that represents one module
  /// @todo: get dimensions from surface
  TGeoMaterial* matAl = new TGeoMaterial("Aluminium", 26, 13, 3);
  TGeoMedium* Al = new TGeoMedium("Aluminium", 2, matAl);
  TGeoVolume* top = gGeoManager->MakeBox("top", Al, 10, 10, 10);
  // recurse through tracking geometry
  dumpTrackingVolume(highestVol, top);
  gGeoManager->SetTopVolume(top);
  gGeoManager->Export(m_gdmlFileName.c_str());
  return StatusCode::SUCCESS;
}

StatusCode TrkGeoToGdmlDumpSvc::finalize() { return Service::finalize(); }
