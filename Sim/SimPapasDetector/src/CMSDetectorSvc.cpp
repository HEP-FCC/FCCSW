#include "CMSDetectorSvc.h"
#include "CMSFieldSvc.h"
#include "papas/detectors/CMS.h"
#include "papas/detectors/Field.h"

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
  //m_field = service(m_fieldServiceName)->field();
  m_papasFieldSvc = service(m_fieldServiceName);
  m_detector = std::make_shared<papas::CMS>(m_cyl1, m_cyl2,1.9, 2.9, m_papasFieldSvc->field());
  return StatusCode::SUCCESS;
}

StatusCode CMSDetSvc::finalize() { return StatusCode::SUCCESS; }
