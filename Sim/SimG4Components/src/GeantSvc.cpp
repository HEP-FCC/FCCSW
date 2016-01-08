#include "GeantSvc.h"
// Gaudi
#include "GaudiKernel/IToolSvc.h"

// Geant
#include "G4Event.hh"
#include "G4VModularPhysicsList.hh"

DECLARE_SERVICE_FACTORY(GeantSvc)

GeantSvc::GeantSvc(const std::string& aName, ISvcLocator* aSL): base_class(aName, aSL) {
  declareProperty("detector", m_detectorTool);
  declarePrivateTool(m_detectorTool);
  declareProperty("physicslist", m_physicsListTool);
  declarePrivateTool(m_physicsListTool);
  declareProperty("actions", m_actionsTool);
  declarePrivateTool(m_actionsTool);
}

GeantSvc::~GeantSvc(){}

StatusCode GeantSvc::initialize(){
  // Initialize necessary Gaudi components
  if (Service::initialize().isFailure()){
    error()<<"Unable to initialize Service()"<<endmsg;
    return StatusCode::FAILURE;
  }
  m_toolSvc = service("ToolSvc");
  if (!m_toolSvc) {
    error()<<"Unable to locate Tool Service"<<endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_detectorTool.retrieve()) {
    error()<<"Unable to retrieve detector construction"<<endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_physicsListTool.retrieve()) {
    error()<<"Unable to retrieve physics list"<<endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_actionsTool.retrieve()) {
    error()<<"Unable to retrieve list of user actions"<<endmsg;
    return StatusCode::FAILURE;
  }

  // Initialize Geant run manager
  // Load physics list, deleted in ~G4RunManager()
  m_runManager.SetUserInitialization(m_physicsListTool->getPhysicsList());
  // Take geometry (from DD4Hep), deleted in ~G4RunManager()
  m_runManager.SetUserInitialization(m_detectorTool->getDetectorConstruction());
  m_runManager.Initialize();
  // Attach user actions
  m_runManager.SetUserInitialization(m_actionsTool->getUserActionInitialization());

  if( !m_runManager.start()) {
    error() << "Unable to initialize GEANT correctly." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
StatusCode GeantSvc::processEvent(G4Event& aEvent) {
  bool status = m_runManager.processEvent( aEvent );
  if ( !status ) {
     error() << "Unable to process event in Geant" << endmsg;
     return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
StatusCode GeantSvc::retrieveEvent(G4Event*& aEvent) {
  return m_runManager.retrieveEvent(aEvent);
}

StatusCode GeantSvc::terminateEvent() {
  m_runManager.terminateEvent();
  return StatusCode::SUCCESS;
}

StatusCode GeantSvc::finalize() {
  m_runManager.finalize();
  return Service::finalize();
}
