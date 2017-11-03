#include "CMSFieldSvc.h"
#include "papas/detectors/VolumeCylinder.h"
#include "papas/detectors/cms/CMSField.h"

DECLARE_SERVICE_FACTORY(CMSFieldSvc)

CMSFieldSvc::CMSFieldSvc(const std::string& name, ISvcLocator* svc) : base_class(name, svc), m_field(nullptr) {}

StatusCode CMSFieldSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_field = std::make_shared<papas::CMSField>(m_fieldMagnitude, m_fieldRadius, m_fieldZ);
  return StatusCode::SUCCESS;
}

StatusCode CMSFieldSvc::finalize() { return StatusCode::SUCCESS; }
