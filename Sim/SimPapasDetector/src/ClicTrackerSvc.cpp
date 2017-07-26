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
  //convert the gaudi property to what we actually require
  std::map<int, std::pair<double, double>> resMap;
  for (const auto v : m_resMap) {
    resMap[std::stoi(v.first)] = v.second;
  }
  for (const auto v : resMap) {
     debug() << v.first << v.second.first << v.second.second << std::endl;
  }
  m_tracker = std::make_shared<papas::ClicTracker>(m_radius,
                                                   m_z,
                                                   m_x0,
                                                   m_lambdaI,
                                                   m_thetaParam,
                                                   resMap,
                                                   m_ptThresholdLow,
                                                   m_ptProbabilityLow,
                                                   m_ptThresholdHigh,
                                                   m_ptProbabilityHigh);
  return StatusCode::SUCCESS;
}

StatusCode ClicTrackerSvc::finalize() { return StatusCode::SUCCESS; }

