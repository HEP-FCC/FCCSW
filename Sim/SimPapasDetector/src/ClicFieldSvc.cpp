#include "ClicFieldSvc.h"
#include "papas/detectors/clic/ClicField.h"
#include "papas/detectors/VolumeCylinder.h"

DECLARE_SERVICE_FACTORY(ClicFieldSvc)

ClicFieldSvc::ClicFieldSvc
(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc), m_field(nullptr)
{
}

StatusCode ClicFieldSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  //auto vol =papas::VolumeCylinder(papas::Layer::kField, 2.4, 5.3); //to pass these pars also
  m_field = std::make_shared<papas::ClicField>(m_fieldMagnitude,
                                               m_fieldRadius,
                                               m_fieldZ,
                                               m_fieldX0,
    m_fieldLambdaI);
  return StatusCode::SUCCESS;
}

StatusCode ClicFieldSvc::finalize() { return StatusCode::SUCCESS; }
