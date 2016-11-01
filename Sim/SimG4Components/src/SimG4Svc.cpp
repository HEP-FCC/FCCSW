#include "SimG4Svc.h"

// Gaudi
#include "GaudiKernel/IToolSvc.h"

// Geant
#include "G4Event.hh"
#include "G4VModularPhysicsList.hh"
#include "G4UImanager.hh"

DECLARE_SERVICE_FACTORY(SimG4Svc)

SimG4Svc::SimG4Svc(const std::string& aName, ISvcLocator* aSL):
  base_class(aName, aSL) {
  declareProperty("detector", m_detectorTool);
  declarePrivateTool(m_detectorTool, "SimG4DD4hepDetector", true);
  declareProperty("physicslist", m_physicsListTool);
  declarePrivateTool(m_physicsListTool, "SimG4FtfpBert", true);
  declareProperty("actions", m_actionsTool);
  declarePrivateTool(m_actionsTool, "SimG4FullSimActions", true);
  declareProperty("magneticField", m_magneticFieldTool);
  declarePrivateTool(m_magneticFieldTool,"SimG4ConstantMagneticFieldTool", true);
  declareProperty("G4commands",m_g4Commands);
  declareProperty("regions",m_regionToolNames);
}

SimG4Svc::~SimG4Svc(){}

StatusCode SimG4Svc::initialize(){
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
  m_runManager.SetUserInitialization(m_physicsListTool->physicsList());
  // Take geometry (from DD4Hep), deleted in ~G4RunManager()
  m_runManager.SetUserInitialization(m_detectorTool->detectorConstruction());

  if (m_g4Commands.size())
  {
    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    for (auto command: m_g4Commands) {
      UImanager->ApplyCommand(command);
    }
  }

  m_runManager.Initialize();
  // Attach user actions
  m_runManager.SetUserInitialization(m_actionsTool->userActionInitialization());
  // Create regions
  for(auto& toolname: m_regionToolNames) {
    ISimG4RegionTool* tool = nullptr;
    if(m_toolSvc->retrieveTool(toolname, tool).isFailure()) {
      error()<<"Unable to retrieve region tool "<<toolname<<endmsg;
      return StatusCode::FAILURE;
    }
    m_regionTools.push_back(tool);
  }
  for(auto& tool: m_regionTools) {
    tool->create();
  }

  if( !m_runManager.start()) {
    error() << "Unable to initialize GEANT correctly." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4Svc::processEvent(G4Event& aEvent) {
  bool status = m_runManager.processEvent( aEvent );
  if ( !status ) {
     error() << "Unable to process event in Geant" << endmsg;
     return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4Svc::retrieveEvent(G4Event*& aEvent) {
  return m_runManager.retrieveEvent(aEvent);
}

StatusCode SimG4Svc::terminateEvent() {
  m_runManager.terminateEvent();
  return StatusCode::SUCCESS;
}

StatusCode SimG4Svc::finalize() {
  m_runManager.finalize();
  return Service::finalize();
}
