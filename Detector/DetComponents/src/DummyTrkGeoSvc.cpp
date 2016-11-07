#include "DummyTrkGeoSvc.h"

// ACTS
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Examples/BuildGenericDetector.hpp"


DECLARE_SERVICE_FACTORY(DummyTrkGeoSvc)

DummyTrkGeoSvc::DummyTrkGeoSvc(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc), m_trackingGeo(nullptr) {}

DummyTrkGeoSvc::~DummyTrkGeoSvc() {}

StatusCode DummyTrkGeoSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_trackingGeo = Acts::buildGenericDetector(Acts::Logging::VERBOSE,
                                   Acts::Logging::VERBOSE,
                                   Acts::Logging::VERBOSE,
                                   0);
 ;
  return StatusCode::SUCCESS;
}

StatusCode DummyTrkGeoSvc::finalize() { return StatusCode::SUCCESS; }
