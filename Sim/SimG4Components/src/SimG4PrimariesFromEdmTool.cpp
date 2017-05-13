// local
#include "SimG4PrimariesFromEdmTool.h"

// FCCSW
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Common/Units.h"

// datamodel
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

// Geant4
#include "G4Event.hh"

// Declaration of the Tool
DECLARE_COMPONENT(SimG4PrimariesFromEdmTool)

SimG4PrimariesFromEdmTool::SimG4PrimariesFromEdmTool(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareProperty("genParticles", m_genParticles, "Handle for the EDM MC particles to be read");
}

SimG4PrimariesFromEdmTool::~SimG4PrimariesFromEdmTool() {}

StatusCode SimG4PrimariesFromEdmTool::initialize() { return GaudiTool::initialize(); }

G4Event* SimG4PrimariesFromEdmTool::g4Event() {
  auto theEvent = new G4Event();
  const fcc::MCParticleCollection* mcparticles = m_genParticles.get();
  for (const auto& mcparticle : *mcparticles) {
    const fcc::ConstGenVertex& v = mcparticle.startVertex();
    G4PrimaryVertex* g4Vertex = new G4PrimaryVertex(v.x() * sim::edm2g4::length,
                                                    v.y() * sim::edm2g4::length,
                                                    v.z() * sim::edm2g4::length,
                                                    v.ctau() * sim::edm2g4::length);
    const fcc::BareParticle& mccore = mcparticle.core();
    G4PrimaryParticle* g4Particle = new G4PrimaryParticle(mccore.pdgId,
                                                          mccore.p4.px * sim::edm2g4::energy,
                                                          mccore.p4.py * sim::edm2g4::energy,
                                                          mccore.p4.pz * sim::edm2g4::energy);
    g4Particle->SetUserInformation(new sim::ParticleInformation(mcparticle));
    g4Vertex->SetPrimary(g4Particle);
    theEvent->AddPrimaryVertex(g4Vertex);
  }
  return theEvent;
}
