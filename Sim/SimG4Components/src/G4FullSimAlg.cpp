#include "G4FullSimAlg.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Interface/IG4SimSvc.h"

// albers
#include "datamodel/MCParticleCollection.h"

// Geant
#include "G4Event.hh"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_ALGORITHM_FACTORY(G4FullSimAlg)

G4FullSimAlg::G4FullSimAlg(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc) {
  declareInput("genParticles", m_genParticles);
  declareProperty("outputTracker",m_saveTrackerTool);
  declarePrivateTool(m_saveTrackerTool);
  declareProperty("outputHCal",m_saveHCalTool);
  declarePrivateTool(m_saveHCalTool);
}
G4FullSimAlg::~G4FullSimAlg() {}

StatusCode G4FullSimAlg::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  m_geantSvc = service("G4SimSvc");
  if (!m_geantSvc) {
    error() << "Unable to locate Geant Simulation Service" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_saveHCalTool.retrieve()) {
    error() << "Unable to retrieve the output saving tool (for HCal)." << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_saveTrackerTool.retrieve()) {
    error() << "Unable to retrieve the output saving tool (for tracker)." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode G4FullSimAlg::execute() {
  // first translate the event
  G4Event* event = EDM2G4();
  if ( !event ) {
    error() << "Unable to translate EDM MC data to G4Event" << endmsg;
    return StatusCode::FAILURE;
  }
  m_geantSvc->processEvent(*event);
  G4Event* constevent;
  m_geantSvc->retrieveEvent(constevent);
  m_saveTrackerTool->saveOutput(*constevent);
  m_saveHCalTool->saveOutput(*constevent);
  m_geantSvc->terminateEvent();
  return StatusCode::SUCCESS;
}

StatusCode G4FullSimAlg::finalize() {
  return GaudiAlgorithm::finalize();
}

G4Event* G4FullSimAlg::EDM2G4() {
  // Event will be passed to G4RunManager and be deleted in G4RunManager::RunTermination()
  G4Event* g4_event = new G4Event();
  // Creating EDM collections
  const MCParticleCollection* mcparticles = m_genParticles.get();
  // Adding one particle per one vertex => vertices repeated
  for(const auto& mcparticle : *mcparticles) {
    const GenVertex& v = mcparticle.read().StartVertex.read();
    G4PrimaryVertex* g4_vertex = new G4PrimaryVertex
      (v.Position.X*sim::edm2g4::length, v.Position.Y*sim::edm2g4::length, v.Position.Z*sim::edm2g4::length, v.Ctau*sim::edm2g4::length);
    const BareParticle& mccore = mcparticle.read().Core;
    G4PrimaryParticle* g4_particle = new G4PrimaryParticle
      (mccore.Type, mccore.P4.Px*sim::edm2g4::energy, mccore.P4.Py*sim::edm2g4::energy, mccore.P4.Pz*sim::edm2g4::energy);
    g4_vertex->SetPrimary(g4_particle);
    g4_event->AddPrimaryVertex(g4_vertex);
  }
  return g4_event;
}

