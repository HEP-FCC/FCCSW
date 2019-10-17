#include "SimG4Common/RunManager.h"

// Geant
#include "G4VModularPhysicsList.hh"

namespace sim {
RunManager::RunManager()
    : G4RunManager(),
      m_prevEventTerminated(true),
      m_msgSvc("MessageSvc", "RunManager"),
      m_log(&(*m_msgSvc), "RunManager") {}

RunManager::~RunManager() {}

StatusCode RunManager::start() {
  // as in G4RunManager::BeamOn()
  if (G4RunManager::ConfirmBeamOnCondition()) {
    G4RunManager::ConstructScoringWorlds();
    G4RunManager::RunInitialization();
    return StatusCode::SUCCESS;
  } else {
    return StatusCode::FAILURE;
  }
}

StatusCode RunManager::processEvent(G4Event& aEvent) {
  if (!m_prevEventTerminated) {
    m_log << MSG::ERROR << "Trying to process an event, but previous event has not been terminated" << endmsg;
    return StatusCode::FAILURE;
  }
  G4RunManager::currentEvent = &aEvent;
  G4RunManager::eventManager->ProcessOneEvent(G4RunManager::currentEvent);
  G4RunManager::AnalyzeEvent(G4RunManager::currentEvent);
  G4RunManager::UpdateScoring();
  m_prevEventTerminated = false;
  return StatusCode::SUCCESS;
}

StatusCode RunManager::retrieveEvent(G4Event*& aEvent) {
  if (m_prevEventTerminated) {
    m_log << MSG::ERROR << "Trying to retrieve an event, but no event has been processed by Geant" << endmsg;
    return StatusCode::FAILURE;
  }
  aEvent = const_cast<G4Event*>(G4RunManager::GetCurrentEvent());
  return StatusCode::SUCCESS;
}

StatusCode RunManager::terminateEvent() {
  if (m_prevEventTerminated) {
    m_log << MSG::ERROR << "Trying to terminate an event, but no event has been processed by Geant" << endmsg;
    return StatusCode::FAILURE;
  }
  G4RunManager::TerminateOneEvent();
  m_prevEventTerminated = true;
  return StatusCode::SUCCESS;
}
void RunManager::finalize() { G4RunManager::RunTermination(); }
}
