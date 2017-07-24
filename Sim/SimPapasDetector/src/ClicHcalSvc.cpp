#include "ClicHcalSvc.h"
#include "papas/detectors/clic/ClicHcal.h"
#include "papas/detectors/VolumeCylinder.h"

DECLARE_SERVICE_FACTORY(ClicHcalSvc)

ClicHcalSvc::ClicHcalSvc
(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc), m_hcal(nullptr)
{
}

StatusCode ClicHcalSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  //auto vol =papas::VolumeCylinder(papas::Layer::kHcal, 2.4, 5.3); //to pass these pars also
  m_hcal = std::make_shared<papas::ClicHCAL>();
  
  return StatusCode::SUCCESS;
}

StatusCode ClicHcalSvc::finalize() { return StatusCode::SUCCESS; }
