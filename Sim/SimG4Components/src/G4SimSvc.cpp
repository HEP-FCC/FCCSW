#include "G4SimSvc.h"
// Gaudi
#include "GaudiKernel/IToolSvc.h"

// Geant
#include "G4Event.hh"
#include "G4VModularPhysicsList.hh"

DECLARE_SERVICE_FACTORY(G4SimSvc)

G4SimSvc::G4SimSvc(const std::string& aName, ISvcLocator* aSL):
  base_class(aName, aSL) {
  declareProperty("detector", m_detectorTool);
  declarePrivateTool(m_detectorTool, "G4DD4hepDetector", true);
  declareProperty("physicslist", m_physicsListTool);
  declarePrivateTool(m_physicsListTool, "G4FTFP_BERT", true);
  declareProperty("actions", m_actionsTool);
  declarePrivateTool(m_actionsTool, "G4FullSimActions", true);
  declareProperty("magneticField", m_magneticFieldTool);
  declarePrivateTool(m_magneticFieldTool,"G4ConstantMagneticFieldTool", true);
}

G4SimSvc::~G4SimSvc(){}

StatusCode G4SimSvc::initialize(){
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
  if (!m_magneticFieldTool.retrieve()) {
    error()<<"Unable to retrieve the magnetic field"<<endmsg;
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
StatusCode G4SimSvc::processEvent(G4Event& aEvent) {
  bool status = m_runManager.processEvent( aEvent );
  if ( !status ) {
     error() << "Unable to process event in Geant" << endmsg;
     return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
StatusCode G4SimSvc::retrieveEvent(G4Event*& aEvent) {
  return m_runManager.retrieveEvent(aEvent);
}

StatusCode G4SimSvc::terminateEvent() {
  m_runManager.terminateEvent();
  return StatusCode::SUCCESS;
}

StatusCode G4SimSvc::finalize() {
  m_runManager.finalize();
  return Service::finalize();
}
