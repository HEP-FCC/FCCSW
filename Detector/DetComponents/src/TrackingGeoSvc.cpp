#include "TrackingGeoSvc.h"

// ACTS
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Detector/TrackingVolume.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Plugins/DD4hepPlugins/DD4hepCylinderGeometryBuilder.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"
#include "ACTS/Plugins/TGeoPlugins/TGeoLayerBuilder.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Material/Material.hpp"
#include "ACTS/Tools/CylinderGeometryBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Tools/LayerCreator.hpp"
#include "ACTS/Tools/PassiveLayerBuilder.hpp"
#include "ACTS/Tools/SurfaceArrayCreator.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"


DECLARE_SERVICE_FACTORY(TrackingGeoSvc)

TrackingGeoSvc::TrackingGeoSvc(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc), m_trackingGeo(nullptr) {}

TrackingGeoSvc::~TrackingGeoSvc() {}

StatusCode TrackingGeoSvc::initialize() {
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service" << endmsg;
    return StatusCode::FAILURE;
  }
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  // hand over LayerArrayCreator
  auto layerArrayCreator = std::make_shared<Acts::LayerArrayCreator>();
  // tracking volume array creator
  auto trackingVolumeArrayCreator = std::make_shared<Acts::TrackingVolumeArrayCreator>();
  // configure the cylinder volume helper
  Acts::CylinderVolumeHelper::Config cvhConfig;
  cvhConfig.layerArrayCreator = layerArrayCreator;
  cvhConfig.trackingVolumeArrayCreator = trackingVolumeArrayCreator;
  auto cylinderVolumeHelper = std::make_shared<Acts::CylinderVolumeHelper>(cvhConfig);
  // configure the volume builder
  Acts::CylinderVolumeBuilder::Config pvbConfig;
  pvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
  auto cylinderVolumeBuilder = std::make_shared<Acts::CylinderVolumeBuilder>(pvbConfig);
  // configure geometry builder with the surface array creator
  Acts::DD4hepCylinderGeometryBuilder::Config cgConfig;
  cgConfig.detWorld = m_geoSvc->getDD4HepGeo();
  cgConfig.volumeHelper = cylinderVolumeHelper;
  cgConfig.volumeBuilder = cylinderVolumeBuilder;
  auto geometryBuilder = std::make_shared<Acts::DD4hepCylinderGeometryBuilder>(cgConfig);

  // set the tracking geometry
  m_trackingGeo = (std::move(geometryBuilder->trackingGeometry()));
  return StatusCode::SUCCESS;
}

StatusCode TrackingGeoSvc::finalize() { return StatusCode::SUCCESS; }
