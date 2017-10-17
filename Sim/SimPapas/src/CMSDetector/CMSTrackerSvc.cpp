#include "CMSTrackerSvc.h"
#include "papas/detectors/VolumeCylinder.h"
#include "papas/detectors/cms/CMSTracker.h"

DECLARE_SERVICE_FACTORY(CMSTrackerSvc)

CMSTrackerSvc::CMSTrackerSvc(const std::string& name, ISvcLocator* svc) : base_class(name, svc), m_tracker(nullptr) {}

StatusCode CMSTrackerSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_tracker = std::make_shared<papas::CMSTracker>(m_radius,
                                                  m_z,
                                                  m_x0,
                                                  m_lambdaI,
                                                  m_resolution,
                                                  m_ptThreshold,
                                                  m_ptThresholdLow,
                                                  m_ptProbabilityLow,
                                                  m_ptThresholdHigh,
                                                  m_ptProbabilityHigh);
  return StatusCode::SUCCESS;
}

StatusCode CMSTrackerSvc::finalize() { return StatusCode::SUCCESS; }
