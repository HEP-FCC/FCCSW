#include "CMSDetectorSvc.h"
#include "CMSFieldSvc.h"
#include "CMSEcalSvc.h"
#include "CMSHcalSvc.h"
#include "CMSTrackerSvc.h"

#include "papas/detectors/cms/CMS.h"

DECLARE_SERVICE_FACTORY(CMSDetSvc)

CMSDetSvc::CMSDetSvc
(const std::string& name, ISvcLocator* svc)
: base_class(name, svc), m_detector(nullptr)
{
}

StatusCode CMSDetSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_papasEcalSvc = service(m_ecalServiceName);
  m_papasHcalSvc = service(m_hcalServiceName);
  m_papasTrackerSvc = service(m_trackerServiceName);
  m_papasFieldSvc = service(m_fieldServiceName);
  m_detector = std::make_shared<papas::CMS>(m_papasEcalSvc->calorimeter(),
                                             m_papasHcalSvc->calorimeter(),
                                             m_papasTrackerSvc->tracker(),
                                             m_papasFieldSvc->field());
  return StatusCode::SUCCESS;
}

StatusCode CMSDetSvc::finalize() { return StatusCode::SUCCESS; }
