#include "CMSDetectorSvc.h"
#include "CMSEcalSvc.h"
#include "CMSFieldSvc.h"
#include "CMSHcalSvc.h"
#include "CMSTrackerSvc.h"

#include "papas/detectors/cms/CMS.h"

DECLARE_SERVICE_FACTORY(CMSDetSvc)

CMSDetSvc::CMSDetSvc(const std::string& name, ISvcLocator* svc) : base_class(name, svc), m_detector(nullptr) {}

StatusCode CMSDetSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  SmartIF<IPapasFieldSvc> papasFieldSvc;
  SmartIF<IPapasCalorimeterSvc> papasEcalSvc;
  SmartIF<IPapasCalorimeterSvc> papasHcalSvc;
  SmartIF<IPapasTrackerSvc> papasTrackerSvc;
  papasEcalSvc = service(m_ecalServiceName);
  papasHcalSvc = service(m_hcalServiceName);
  papasTrackerSvc = service(m_trackerServiceName);
  papasFieldSvc = service(m_fieldServiceName);
  m_detector = std::make_shared<papas::CMS>(papasEcalSvc->calorimeter(),
                                            papasHcalSvc->calorimeter(),
                                            papasTrackerSvc->tracker(),
                                            papasFieldSvc->field(),
                                            m_electronAcceptanceMagnitude,
                                            m_electronAcceptanceEta,
                                            m_muonAcceptanceMagnitude,
                                            m_muonAcceptanceTheta,
                                            m_electronEnergyFactor,
                                            m_muonResolution);

  return StatusCode::SUCCESS;
}

StatusCode CMSDetSvc::finalize() { return StatusCode::SUCCESS; }
