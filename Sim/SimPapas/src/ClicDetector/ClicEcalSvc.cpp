#include "ClicEcalSvc.h"
#include "papas/detectors/VolumeCylinder.h"
#include "papas/detectors/clic/ClicEcal.h"

DECLARE_SERVICE_FACTORY(ClicEcalSvc)

ClicEcalSvc::ClicEcalSvc(const std::string& name, ISvcLocator* svc) : base_class(name, svc), m_ecal(nullptr) {}

StatusCode ClicEcalSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_ecal = std::make_shared<papas::ClicECAL>(m_innerRadius,
                                             m_innerZ,
                                             m_depth,
                                             m_clusterSizePhoton,
                                             m_clusterSize,
                                             m_etaAcceptance,
                                             m_emin,
                                             m_eresBarrel,
                                             m_nX0,
                                             m_nLambdaI,
                                             m_eResponse);
  return StatusCode::SUCCESS;
}

StatusCode ClicEcalSvc::finalize() { return StatusCode::SUCCESS; }
