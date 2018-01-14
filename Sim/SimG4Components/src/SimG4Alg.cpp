#include "SimG4Alg.h"

// FCCSW
#include "SimG4Common/EventInformation.h"
#include "SimG4Interface/ISimG4SaveHistoryTool.h"
#include "SimG4Interface/ISimG4Svc.h"

// Geant
#include "G4Event.hh"

DECLARE_ALGORITHM_FACTORY(SimG4Alg)

SimG4Alg::SimG4Alg(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("eventProvider", m_eventTool, "Handle for tool that creates the G4Event");
  declareProperty("saveHistoryTool", m_historyTool, "Handle for tool that saves MC truth history");
}
SimG4Alg::~SimG4Alg() {}

StatusCode SimG4Alg::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;
  m_geantSvc = service("SimG4Svc");
  if (!m_geantSvc) {
    error() << "Unable to locate Geant Simulation Service" << endmsg;
    return StatusCode::FAILURE;
  }
  for (auto& toolname : m_saveToolNames) {
    m_saveTools.push_back(tool<ISimG4SaveOutputTool>(toolname));
    // FIXME: check StatusCode once the m_saveTools is a ToolHandleArray
    // if (!) {
    //   error() << "Unable to retrieve the output saving tool." << endmsg;
    //   return StatusCode::FAILURE;
    // }
  }
  if (!m_eventTool.retrieve()) {
    error() << "Unable to retrieve the G4Event provider " << m_eventTool << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_historyTool.retrieve()) {
    warning() << "Unable to retrieve history saving tool, will not save any history" << endmsg;
    m_saveHistory = false;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4Alg::execute() {
  // first translate the event
  G4Event* event = m_eventTool->g4Event();
  auto eventInfo = new sim::EventInformation();
  // here the event takes ownership of the event information
  event->SetUserInformation(eventInfo);

  if (!event) {
    error() << "Unable to retrieve G4Event from " << m_eventTool << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_saveHistory) {
    m_historyTool->reset(eventInfo);
  }
  m_geantSvc->processEvent(*event);
  G4Event* constevent;
  m_geantSvc->retrieveEvent(constevent);
  for (auto& tool : m_saveTools) {
    tool->saveOutput(*constevent);
  }
  if (m_saveHistory) {
    m_historyTool->saveOutput(*constevent);
  }
  m_geantSvc->terminateEvent();
  return StatusCode::SUCCESS;
}

StatusCode SimG4Alg::finalize() { return GaudiAlgorithm::finalize(); }
