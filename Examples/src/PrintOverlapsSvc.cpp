#include "PrintOverlapsSvc.h"

#include "TGeoManager.h"

DECLARE_SERVICE_FACTORY(PrintOverlapsSvc)

PrintOverlapsSvc::PrintOverlapsSvc(const std::string& aName, ISvcLocator* aSL) : base_class(aName, aSL) {}

StatusCode PrintOverlapsSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  if (nullptr == gGeoManager) {
    error() << "Unable to find pointer to global geometry manager" << endmsg;
    return StatusCode::FAILURE;
  }
  gGeoManager->CheckOverlaps();
  gGeoManager->PrintOverlaps();
  return StatusCode::SUCCESS;
}

StatusCode PrintOverlapsSvc::finalize() { return Service::finalize(); }
