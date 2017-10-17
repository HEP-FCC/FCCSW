#include "ClicDetectorSvc.h"
#include "ClicEcalSvc.h"
#include "ClicFieldSvc.h"
#include "ClicHcalSvc.h"
#include "ClicTrackerSvc.h"

#include "papas/detectors/clic/Clic.h"

DECLARE_SERVICE_FACTORY(ClicDetSvc)

ClicDetSvc::ClicDetSvc(const std::string& name, ISvcLocator* svc) : base_class(name, svc), m_detector(nullptr) {}

StatusCode ClicDetSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  /// Pointer to the interface of papas detector
  SmartIF<IPapasFieldSvc> papasFieldSvc;
  SmartIF<IPapasCalorimeterSvc> papasEcalSvc;
  SmartIF<IPapasCalorimeterSvc> papasHcalSvc;
  SmartIF<IPapasTrackerSvc> papasTrackerSvc;
  papasEcalSvc = service(m_ecalServiceName);
  papasHcalSvc = service(m_hcalServiceName);
  papasTrackerSvc = service(m_trackerServiceName);
  papasFieldSvc = service(m_fieldServiceName);
  m_detector = std::make_shared<papas::Clic>(papasEcalSvc->calorimeter(),
                                             papasHcalSvc->calorimeter(),
                                             papasTrackerSvc->tracker(),
                                             papasFieldSvc->field(),
                                             m_electronAcceptanceMagnitude,
                                             m_electronAcceptanceEta,
                                             m_electronAcceptanceEfficiency,
                                             m_muonAcceptanceMagnitude,
                                             m_muonAcceptanceTheta,
                                             m_muonResolution);
  return StatusCode::SUCCESS;
}

StatusCode ClicDetSvc::finalize() { return StatusCode::SUCCESS; }
