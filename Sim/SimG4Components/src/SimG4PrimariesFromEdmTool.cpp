// local
#include "SimG4PrimariesFromEdmTool.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// FCCSW
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Common/Units.h"

// datamodel
#include "edm4hep/MCParticleCollection.h"

// Geant4
#include "G4Event.hh"

// Declaration of the Tool
DECLARE_COMPONENT(SimG4PrimariesFromEdmTool)

SimG4PrimariesFromEdmTool::SimG4PrimariesFromEdmTool(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareProperty("GenParticles", m_genParticles, "Handle for the EDM MC particles to be read");
}

SimG4PrimariesFromEdmTool::~SimG4PrimariesFromEdmTool() {}

StatusCode SimG4PrimariesFromEdmTool::initialize() { return GaudiTool::initialize(); }

G4Event* SimG4PrimariesFromEdmTool::g4Event() {
  auto theEvent = new G4Event();
  const edm4hep::MCParticleCollection* mcparticles = m_genParticles.get();
  for (const auto& mcparticle : *mcparticles) {
    const Vector3d v = mcparticle.getVertex();
    G4PrimaryVertex* g4Vertex = new G4PrimaryVertex(v.x * sim::edm2g4::length,
                                                    v.y * sim::edm2g4::length,
                                                    v.z * sim::edm2g4::length,
                                                    mcparticle.getTime() / Gaudi::Units::c_light  * sim::edm2g4::length);
    const Vector3d p = mcparticle.getMomentum();
    G4PrimaryParticle* g4Particle = new G4PrimaryParticle(mcparticle.getPDG(),
                                                          mom.x * sim::edm2g4::energy,
                                                          mom.y * sim::edm2g4::energy,
                                                          mom.z * sim::edm2g4::energy);
    g4Particle->SetUserInformation(new sim::ParticleInformation(mcparticle));
    g4Vertex->SetPrimary(g4Particle);
    theEvent->AddPrimaryVertex(g4Vertex);
  }
  return theEvent;
}
