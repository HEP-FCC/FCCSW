#include "CMSFieldSvc.h"
#include "papas/detectors/CMSField.h"
#include "papas/detectors/VolumeCylinder.h"

DECLARE_SERVICE_FACTORY(CMSFieldSvc)

CMSFieldSvc::CMSFieldSvc
(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc), m_field(nullptr)
{
}

StatusCode CMSFieldSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  auto vol =papas::VolumeCylinder(papas::Layer::kField, 2.9, 3.6); //to pass these pars also
  m_field = std::make_shared<papas::CMSField>(std::move(vol), m_fieldMagnitude);
  
  return StatusCode::SUCCESS;
}

StatusCode CMSFieldSvc::finalize() { return StatusCode::SUCCESS; }
