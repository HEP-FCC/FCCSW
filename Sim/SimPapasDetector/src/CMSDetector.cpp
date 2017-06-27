#include "CMSDetector.h"

#include "papas/detectors/CMS.h"

DECLARE_SERVICE_FACTORY(CMSDetSvc)

CMSDetSvc::CMSDetSvc
(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc), m_detector(nullptr)
{
  //declareProperty("innerEcal", m_innerEcal, "Ecal inner cylinder");
  //declareProperty("outerEcal", m_outerEcal, "Ecal inner cylinder");
  }


StatusCode CMSDetSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_detector = std::make_shared<papas::CMS>(m_cyl1, m_cyl2);
  return StatusCode::SUCCESS;
}

StatusCode CMSDetSvc::finalize() { return StatusCode::SUCCESS; }
