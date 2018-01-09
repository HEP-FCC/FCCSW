#include "ClicFieldSvc.h"
#include "papas/detectors/VolumeCylinder.h"
#include "papas/detectors/clic/ClicField.h"

DECLARE_SERVICE_FACTORY(ClicFieldSvc)

ClicFieldSvc::ClicFieldSvc(const std::string& name, ISvcLocator* svc) : base_class(name, svc), m_field(nullptr) {}

StatusCode ClicFieldSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_field = std::make_shared<papas::ClicField>(m_fieldMagnitude, m_fieldRadius, m_fieldZ);
  return StatusCode::SUCCESS;
}

StatusCode ClicFieldSvc::finalize() { return StatusCode::SUCCESS; }
