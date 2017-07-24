#include "ClicDetectorSvc.h"
#include "ClicFieldSvc.h"
#include "ClicEcalSvc.h"
#include "ClicHcalSvc.h"
#include "ClicTrackerSvc.h"

#include "papas/detectors/clic/Clic.h"

DECLARE_SERVICE_FACTORY(ClicDetSvc)

ClicDetSvc::ClicDetSvc
(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc), m_detector(nullptr)
{
}

StatusCode ClicDetSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_papasEcalSvc = service(m_ecalServiceName);
  m_papasHcalSvc = service(m_hcalServiceName);
  m_papasTrackerSvc = service(m_trackerServiceName);
  m_papasFieldSvc = service(m_fieldServiceName);
  m_detector = std::make_shared<papas::Clic>(m_papasEcalSvc->calorimeter(),
                                             m_papasHcalSvc->calorimeter(),
                                             m_papasTrackerSvc->tracker(),
                                             m_papasFieldSvc->field());
  return StatusCode::SUCCESS;
}

StatusCode ClicDetSvc::finalize() { return StatusCode::SUCCESS; }
