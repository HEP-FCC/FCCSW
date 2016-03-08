#include "G4SimAlg.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Interface/IG4SimSvc.h"

// datamodel
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

// Geant
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4RunManager.hh"

DECLARE_ALGORITHM_FACTORY(G4SimAlg)

G4SimAlg::G4SimAlg(const std::string& aName, ISvcLocator* aSvcLoc):
  GaudiAlgorithm(aName, aSvcLoc) {
  declareInput("genParticles", m_genParticles);
  declareProperty("outputs",m_saveToolNames);
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
  return StatusCode::SUCCESS;
}

StatusCode G4SimAlg::execute() {
  // first translate the event
  G4Event* event = nullptr;
  
  G4RunManager* runManager=G4RunManager::GetRunManager();
  G4VUserPrimaryGeneratorAction* generator=const_cast<G4VUserPrimaryGeneratorAction*>(runManager->GetUserPrimaryGeneratorAction());
  if (generator)
  {
 	debug()<<"The generator is set: run GeneratePrimaries() "<<endmsg;
        event=new G4Event();
	generator->GeneratePrimaries(event);
  }
  
  if ( !event ) {
    error() << "Unable to translate EDM MC data to G4Event" << endmsg;
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
