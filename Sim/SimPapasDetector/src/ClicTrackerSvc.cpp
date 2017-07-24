#include "ClicTrackerSvc.h"
#include "papas/detectors/clic/ClicTracker.h"
#include "papas/detectors/VolumeCylinder.h"

DECLARE_SERVICE_FACTORY(ClicTrackerSvc)

ClicTrackerSvc::ClicTrackerSvc
(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc), m_tracker(nullptr)
{
}

StatusCode ClicTrackerSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  //auto vol =papas::VolumeCylinder(papas::Layer::kTracker, 2.4, 5.3); //to pass these pars also
  m_tracker = std::make_shared<papas::ClicTracker>();
  
  return StatusCode::SUCCESS;
}

StatusCode ClicTrackerSvc::finalize() { return StatusCode::SUCCESS; }
