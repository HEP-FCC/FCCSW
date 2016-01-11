#include "G4FastSimAlg.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Interface/IG4SimSvc.h"

// albers
#include "datamodel/MCParticleCollection.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCAssociationCollection.h"

// Geant
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"

DECLARE_ALGORITHM_FACTORY(G4FastSimAlg)

G4FastSimAlg::G4FastSimAlg(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc) {
  declareInput("genParticles", m_genParticles);
  declareOutput("particles", m_recphandle);
  declareOutput("particleassociation", m_partassociationhandle);
}
G4FastSimAlg::~G4FastSimAlg() {}

StatusCode G4FastSimAlg::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  m_geantSvc = service("G4SimSvc");
  if (! m_geantSvc) {
    error() << "Unable to locate Geant Simulation Service" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode G4FastSimAlg::execute() {
  // first translate the event
  G4Event* event = EDM2G4();
  if ( !event ) {
    error() << "Unable to translate EDM MC data to G4Event" << endmsg;
    return StatusCode::FAILURE;
  }
  m_geantSvc->processEvent(*event);
  // G4Event* constevent;
  // m_geantSvc->retrieveEvent(constevent);
  // // here save the output from constevent
  m_geantSvc->terminateEvent();
  return StatusCode::SUCCESS;
}

StatusCode G4FastSimAlg::finalize() {
  return GaudiAlgorithm::finalize();
}

G4Event* G4FastSimAlg::EDM2G4() {
  // Event will be passed to G4RunManager and be deleted in G4RunManager::RunTermination()
  G4Event* g4_event = new G4Event();
  // Creating EDM collections
  const MCParticleCollection& mcparticles = *(m_genParticles.get());
  ParticleCollection* particles = new ParticleCollection();
  ParticleMCAssociationCollection* associations = new ParticleMCAssociationCollection();
  // Adding one particle per one vertex => vertices repeated
  for(const auto& mcparticle : mcparticles) {
    const GenVertex& v = mcparticle.read().StartVertex.read();
    G4PrimaryVertex* g4_vertex = new G4PrimaryVertex
      (v.Position.X*sim::edm2g4::length, v.Position.Y*sim::edm2g4::length, v.Position.Z*sim::edm2g4::length, v.Ctau*sim::edm2g4::length);
    const BareParticle& mccore = mcparticle.read().Core;
    G4PrimaryParticle* g4_particle = new G4PrimaryParticle
      (mccore.Type, mccore.P4.Px*sim::edm2g4::energy, mccore.P4.Py*sim::edm2g4::energy, mccore.P4.Pz*sim::edm2g4::energy);
    ParticleHandle particle = particles->create();
    g4_particle->SetUserInformation(new sim::ParticleInformation(mcparticle, particle));
    ParticleMCAssociationHandle association = associations->create();
    association.mod().Rec = particle;
    association.mod().Sim = mcparticle;
    g4_vertex->SetPrimary(g4_particle);
    g4_event->AddPrimaryVertex(g4_vertex);
  }
  m_recphandle.put(particles);
  m_partassociationhandle.put(associations);
  return g4_event;
}
