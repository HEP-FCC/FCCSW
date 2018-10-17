#include "ClicHcalSvc.h"
#include "papas/detectors/VolumeCylinder.h"
#include "papas/detectors/clic/ClicHcal.h"

DECLARE_SERVICE_FACTORY(ClicHcalSvc)

ClicHcalSvc::ClicHcalSvc(const std::string& name, ISvcLocator* svc) : base_class(name, svc), m_hcal(nullptr) {}

StatusCode ClicHcalSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hcal = std::make_shared<papas::ClicHCAL>(m_innerRadius,
                                             m_innerZ,
                                             m_outerRadius,
                                             m_outerZ,
                                             m_clusterSize,
                                             m_eresBarrel,
                                             m_X0,
                                             m_lambdaI,
                                             m_eResponse,
                                             m_etaAcceptance);
  return StatusCode::SUCCESS;
}

StatusCode ClicHcalSvc::finalize() { return StatusCode::SUCCESS; }
