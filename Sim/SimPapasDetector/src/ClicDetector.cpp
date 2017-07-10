#include "ClicDetector.h"
#include "Clic.h"

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
  m_detector = std::make_shared<papas::Clic>(m_cyl1, m_cyl2);
  return StatusCode::SUCCESS;
}

StatusCode ClicDetSvc::finalize() { return StatusCode::SUCCESS; }
