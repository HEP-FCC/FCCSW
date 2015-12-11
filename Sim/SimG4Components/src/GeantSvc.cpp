#include "GeantSvc.h"
// Gaudi
#include "GaudiKernel/IToolSvc.h"
// FCCSW
#include "DetDesInterfaces/IGeoSvc.h"
#include "SimG4Interface/IGeantConfigTool.h"

// Geant
#include "G4Event.hh"
#include "G4VModularPhysicsList.hh"

DECLARE_SERVICE_FACTORY(sim::GeantSvc)

namespace sim {
GeantSvc::GeantSvc(const std::string& aName, ISvcLocator* aSL): base_class(aName, aSL) {
  declareProperty("config", m_geantConfigName = "GeantFullSimConfig", "Configuration file" );
}

GeantSvc::~GeantSvc(){}

StatusCode GeantSvc::initialize(){
  // Initialize necessary Gaudi components
  if (Service::initialize().isFailure()){
    error()<<"Unable to initialize Service()"<<endmsg;
    return StatusCode::FAILURE;
  }
  if (service("ToolSvc", m_toolSvc, true).isFailure()) {
    error()<<"Unable to locate Tool Service"<<endmsg;
    return StatusCode::FAILURE;
  }
  if (service("GeoSvc", m_geoSvc, true).isFailure()) {
    error()<<"Unable to locate Geometry Service"<<endmsg;
    return StatusCode::FAILURE;
  }
  if (m_toolSvc->retrieveTool(m_geantConfigName, m_geantConfigTool).isFailure()) {
    error()<<"Unable to locate Geometry Service"<<endmsg;
    return StatusCode::FAILURE;
  }
  // Initialize Geant run manager
  // Initialization - Geant part
  // Load physics list, deleted in ~G4RunManager()
  m_runManager.SetUserInitialization(m_geantConfigTool->getPhysicsList());

  // Take geometry (from DD4Hep), deleted in ~G4RunManager()
  m_runManager.SetUserInitialization(m_geoSvc->getGeant4Geo());

  m_runManager.Initialize();
  // Attach user actions
  m_runManager.SetUserInitialization(m_geantConfigTool->getActionInitialization());

  m_geantConfigTool->getOtherSettings();

  if( !m_runManager.start()) {
    error() << "Unable to initialize GEANT correctly." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
StatusCode GeantSvc::processEvent(G4Event* aEvent) {
  bool status = m_runManager.processEvent( aEvent );
  if ( !status ) {
     error() << "Unable to process event in Geant" << endmsg;
     return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
StatusCode GeantSvc::retrieveEvent(const G4Event*& aEvent) {
  return m_runManager.retrieveEvent(aEvent);
}

StatusCode GeantSvc::terminateEvent() {
  m_runManager.terminateEvent();
  return StatusCode::SUCCESS;
}
StatusCode GeantSvc::finalize() {
  m_runManager.finalize();
  return StatusCode::SUCCESS;
}
}
