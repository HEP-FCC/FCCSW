#include "CMSEcalSvc.h"
#include "papas/detectors/VolumeCylinder.h"
#include "papas/detectors/cms/CMSEcal.h"

DECLARE_SERVICE_FACTORY(CMSEcalSvc)

CMSEcalSvc::CMSEcalSvc(const std::string& name, ISvcLocator* svc) : base_class(name, svc), m_ecal(nullptr) {}

StatusCode CMSEcalSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_ecal = std::make_shared<papas::CMSECAL>(m_innerRadius,
                                            m_innerZ,
                                            m_outerRadius,
                                            m_outerZ,
                                            m_x0,
                                            m_lambdaI,
                                            m_clusterSizePhoton,
                                            m_clusterSize,
                                            m_etaCrack,
                                            m_etaAcceptance,
                                            m_ptAcceptance,
                                            m_etaEndcapMin,
                                            m_etaEndcapMax,
                                            m_emin,
                                            m_eres,
                                            m_eresp);
  return StatusCode::SUCCESS;
}

StatusCode CMSEcalSvc::finalize() { return StatusCode::SUCCESS; }
