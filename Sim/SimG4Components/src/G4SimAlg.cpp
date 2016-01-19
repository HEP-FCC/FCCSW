#include "G4SimAlg.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Interface/IG4SimSvc.h"

// albers
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

// Geant
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"

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
  // if (!) {
  //   error() << "Unable to retrieve the output saving tool." << endmsg;
  //   return StatusCode::FAILURE;
  // }
  }
  return StatusCode::SUCCESS;
}

StatusCode G4SimAlg::execute() {
  // first translate the event
  G4Event* event = EDM2G4();
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

G4Event* G4SimAlg::EDM2G4() {
  // Event will be passed to G4RunManager and be deleted in G4RunManager::RunTermination()
  G4Event* g4_event = new G4Event();
  // Creating EDM collections
  const MCParticleCollection& mcparticles = *(m_genParticles.get());
  // Adding one particle per one vertex => vertices repeated
  for(const auto& mcparticle : mcparticles) {
    const ConstGenVertex& v = mcparticle.StartVertex();
    G4PrimaryVertex* g4_vertex = new G4PrimaryVertex
      (v.Position().X*sim::edm2g4::length, v.Position().Y*sim::edm2g4::length, v.Position().Z*sim::edm2g4::length, v.Ctau()*sim::edm2g4::length);
    const BareParticle& mccore = mcparticle.Core();
    G4PrimaryParticle* g4_particle = new G4PrimaryParticle
      (mccore.Type, mccore.P4.Px*sim::edm2g4::energy, mccore.P4.Py*sim::edm2g4::energy, mccore.P4.Pz*sim::edm2g4::energy);
    g4_particle->SetUserInformation(new sim::ParticleInformation(mcparticle));
    g4_vertex->SetPrimary(g4_particle);
    g4_event->AddPrimaryVertex(g4_vertex);
  }
  return g4_event;
}
