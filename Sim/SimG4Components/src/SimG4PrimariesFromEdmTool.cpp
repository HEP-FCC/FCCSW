// local
#include "SimG4PrimariesFromEdmTool.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/ParticleInformation.h"

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
  declareInput("genParticles", m_genParticles, "allGenParticles");
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

SimG4PrimariesFromEdmTool::~SimG4PrimariesFromEdmTool() {}

StatusCode SimG4PrimariesFromEdmTool::initialize() { return GaudiTool::initialize(); }

G4Event* SimG4PrimariesFromEdmTool::g4Event() {
  auto theEvent = new G4Event();
  const fcc::MCParticleCollection* mcparticles = m_genParticles.get();
  for (const auto& mcparticle : *mcparticles) {
    const fcc::ConstGenVertex& v = mcparticle.StartVertex();
    G4PrimaryVertex* g4_vertex = new G4PrimaryVertex(v.Position().X * sim::edm2g4::length,
                                                     v.Position().Y * sim::edm2g4::length,
                                                     v.Position().Z * sim::edm2g4::length,
                                                     v.Ctau() * sim::edm2g4::length);
    const fcc::BareParticle& mccore = mcparticle.Core();
    G4PrimaryParticle* g4_particle = new G4PrimaryParticle(mccore.Type,
                                                           mccore.P4.Px * sim::edm2g4::energy,
                                                           mccore.P4.Py * sim::edm2g4::energy,
                                                           mccore.P4.Pz * sim::edm2g4::energy);
    g4_particle->SetUserInformation(new sim::ParticleInformation(mcparticle));
    g4_vertex->SetPrimary(g4_particle);
    theEvent->AddPrimaryVertex(g4_vertex);
  }
  return theEvent;
}
