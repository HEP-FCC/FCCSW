#include "TrackingGeoSvc.h"

// ACTS
#include "ACTS/Plugins/DD4hepPlugins/ConvertDD4hepDetector.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"


DECLARE_SERVICE_FACTORY(TrackingGeoSvc)

TrackingGeoSvc::TrackingGeoSvc(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc), m_trackingGeo(nullptr) {}

TrackingGeoSvc::~TrackingGeoSvc() {}

StatusCode TrackingGeoSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service" << endmsg;
    return StatusCode::FAILURE;
  }
  m_trackingGeo = std::move(Acts::convertDD4hepDetector(
        m_geoSvc->getDD4HepGeo(),Acts::Logging::VERBOSE, Acts::equidistant, Acts::equidistant, Acts::equidistant));
  return StatusCode::SUCCESS;
}

StatusCode TrackingGeoSvc::finalize() { return StatusCode::SUCCESS; }
