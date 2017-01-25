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
  /// @todo: this is an ad-hoc conversion between gaudi and acts msg levels.
  // A proper logger for acts should be implemented, and this removed.
  Acts::Logging::Level geoMsgLevel;
  switch  (msgLevel()) {
    case (MSG::DEBUG):
      geoMsgLevel = Acts::Logging::DEBUG;
      break;
    case (MSG::VERBOSE):
      geoMsgLevel = Acts::Logging::VERBOSE;
      break;
    case (MSG::INFO):
      geoMsgLevel = Acts::Logging::INFO;
      break;
    case (MSG::WARNING):
      geoMsgLevel = Acts::Logging::WARNING;
      break;
    case (MSG::FATAL):
      geoMsgLevel = Acts::Logging::FATAL;
      break;
    case (MSG::ERROR):
      geoMsgLevel = Acts::Logging::ERROR;
      break;
    default:
      geoMsgLevel = Acts::Logging::VERBOSE;
  }
  m_trackingGeo = std::move(Acts::convertDD4hepDetector(
        m_geoSvc->getDD4HepGeo(), geoMsgLevel, Acts::equidistant, Acts::equidistant, Acts::equidistant));
  return StatusCode::SUCCESS;
}

StatusCode TrackingGeoSvc::finalize() { return StatusCode::SUCCESS; }
