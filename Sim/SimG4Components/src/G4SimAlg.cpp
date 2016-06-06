#include "G4SimAlg.h"

// FCCSW
#include "SimG4Interface/IG4SimSvc.h"

// Geant
#include "G4Event.hh"

DECLARE_ALGORITHM_FACTORY(G4SimAlg)

G4SimAlg::G4SimAlg(const std::string& aName, ISvcLocator* aSvcLoc):
  GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("outputs",m_saveToolNames);
  declarePrivateTool(m_eventGenTool, "G4PrimariesFromEdmTool", true);
  declareProperty("eventGenerator", m_eventGenTool);
}
G4SimAlg::~G4SimAlg() {}

StatusCode G4SimAlg::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  m_geantSvc = service("G4SimSvc");
  if (! m_geantSvc) {
    error() << "Unable to locate Geant Simulation Service" << endmsg;
    return StatusCode::FAILURE;
  }
  for(auto& toolname: m_saveToolNames) {
    m_saveTools.push_back(tool<IG4SaveOutputTool>(toolname));
    // FIXME: check StatusCode once the m_saveTools is a ToolHandleArray
    // if (!) {
    //   error() << "Unable to retrieve the output saving tool." << endmsg;
    //   return StatusCode::FAILURE;
    // }
  }
  if (!m_eventGenTool.retrieve()) {
    error()<<"Unable to retrieve the G4Event generator " << m_eventGenTool <<endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode G4SimAlg::execute() {
  // first translate the event
  G4Event* event = m_eventGenTool->g4Event();

  if ( !event ) {
    error() << "Unable to retrieve G4Event from " << m_eventGenTool << endmsg;
    return StatusCode::FAILURE;
  }
  m_geantSvc->processEvent(*event);
  G4Event* constevent;
  m_geantSvc->retrieveEvent(constevent);
  for(auto& tool: m_saveTools) {
    tool->saveOutput(*constevent);
  }
  m_geantSvc->terminateEvent();
  return StatusCode::SUCCESS;
}

StatusCode G4SimAlg::finalize() {
  return GaudiAlgorithm::finalize();
}
