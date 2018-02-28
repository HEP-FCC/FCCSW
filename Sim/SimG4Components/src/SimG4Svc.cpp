#include "SimG4Svc.h"

// Gaudi
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/IToolSvc.h"

// Geant
#include "G4Event.hh"
#include "G4HadronicProcessStore.hh"
#include "G4UImanager.hh"
#include "G4UIsession.hh"
#include "G4UIterminal.hh"
#include "G4VModularPhysicsList.hh"
#include "G4VisExecutive.hh"
#include "G4VisManager.hh"

DECLARE_SERVICE_FACTORY(SimG4Svc)

SimG4Svc::SimG4Svc(const std::string& aName, ISvcLocator* aSL) : base_class(aName, aSL) {
  declareProperty("detector", m_detectorTool, "Handle for the detector construction tool");
  declareProperty("physicslist", m_physicsListTool, "Handle for the Geant physics list tool");
  declareProperty("actions", m_actionsTool, "Handle for the user action initialization");
  declareProperty("magneticField", m_magneticFieldTool, "Handle for the magnetic field initialization");
}

SimG4Svc::~SimG4Svc() {}

StatusCode SimG4Svc::initialize() {
  // Initialize necessary Gaudi components
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_toolSvc = service("ToolSvc");
  if (!m_toolSvc) {
    error() << "Unable to locate Tool Service" << endmsg;
    return StatusCode::FAILURE;
  }
  m_randSvc = service("RndmGenSvc");
  if (!m_randSvc) {
    error() << "Unable to locate RndmGen Service" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_detectorTool.retrieve()) {
    error() << "Unable to retrieve detector construction" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_physicsListTool.retrieve()) {
    error() << "Unable to retrieve physics list" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_actionsTool.retrieve()) {
    error() << "Unable to retrieve list of user actions" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_magneticFieldTool.retrieve()) {
    error() << "Unable to retrieve the magnetic field" << endmsg;
    return StatusCode::FAILURE;
  }

  // Initialize Geant run manager
  // Load physics list, deleted in ~G4RunManager()
  m_runManager.SetUserInitialization(m_physicsListTool->physicsList());
  // Take geometry (from DD4Hep), deleted in ~G4RunManager()
  m_runManager.SetUserInitialization(m_detectorTool->detectorConstruction());

  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  for (auto command : m_g4PreInitCommands) {
    UImanager->ApplyCommand(command);
  }

  m_runManager.Initialize();

  if (m_interactiveMode) {
    m_visManager = std::make_unique<G4VisExecutive>();
    m_visManager->Initialize();

    m_session = std::make_unique<G4UIterminal>();
    m_session->SessionStart();
  }

  // Attach user actions
  m_runManager.SetUserInitialization(m_actionsTool->userActionInitialization());
  if (!msgLevel(MSG::DEBUG)) {
    G4HadronicProcessStore::Instance()->SetVerbose(0);
    UImanager->ApplyCommand("/run/verbose 0");
    UImanager->ApplyCommand("/process/em/verbose 0");
  }
  // Create regions
  for (auto& toolname : m_regionToolNames) {
    ISimG4RegionTool* tool = nullptr;
    if (m_toolSvc->retrieveTool(toolname, tool).isFailure()) {
      error() << "Unable to retrieve region tool " << toolname << endmsg;
      return StatusCode::FAILURE;
    }
    m_regionTools.push_back(tool);
  }
  for (auto& tool : m_regionTools) {
    tool->create();
  }
  for (auto command : m_g4PostInitCommands) {
    UImanager->ApplyCommand(command);
  }

  // configure the random service
  if (m_rndmFromGaudi) {
    std::vector<long> seedsVec;
    m_randSvc->engine()->seeds(seedsVec);
    long seedsList[] = {seedsVec[0], seedsVec[1]};
    CLHEP::HepRandom::setTheSeeds(seedsList);
  }
  info() << "Random numbers seeds: " << CLHEP::HepRandom::getTheSeeds()[0] << "\t" << CLHEP::HepRandom::getTheSeeds()[1]
         << endmsg;

  if (!m_runManager.start()) {
    error() << "Unable to initialize GEANT correctly." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4Svc::processEvent(G4Event& aEvent) {
  bool status = m_runManager.processEvent(aEvent);
  if (!status) {
    error() << "Unable to process event in Geant" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4Svc::retrieveEvent(G4Event*& aEvent) { return m_runManager.retrieveEvent(aEvent); }

StatusCode SimG4Svc::terminateEvent() {
  m_runManager.terminateEvent();
  return StatusCode::SUCCESS;
}

StatusCode SimG4Svc::finalize() {
  m_runManager.finalize();
  return Service::finalize();
}
