// local
#include "SimG4GeantinosFromEdmTool.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// FCCSW
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Common/Units.h"

// datamodel
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

// Geant4
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

// Declaration of the Tool
DECLARE_COMPONENT(SimG4GeantinosFromEdmTool)

SimG4GeantinosFromEdmTool::SimG4GeantinosFromEdmTool(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareProperty("genParticles", m_genParticles, "Handle for the EDM MC particles to be read");
}

SimG4GeantinosFromEdmTool::~SimG4GeantinosFromEdmTool() {}

StatusCode SimG4GeantinosFromEdmTool::initialize() { return GaudiTool::initialize(); }

G4Event* SimG4GeantinosFromEdmTool::g4Event() {
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  auto theEvent = new G4Event();

  G4ParticleDefinition* particleDefPos = particleTable->FindParticle("chargedgeantino");
  G4ParticleDefinition* particleDefNeg = particleTable->FindParticle("chargedgeantinonegative");
  G4ParticleDefinition* particleDefNeutral = particleTable->FindParticle("geantino");

  const fcc::MCParticleCollection* mcparticles = m_genParticles.get();
  for (const auto& mcparticle : *mcparticles) {
    const fcc::ConstGenVertex& v = mcparticle.startVertex();
    G4PrimaryVertex* g4Vertex = new G4PrimaryVertex(v.x() * sim::edm2g4::length,
                                                    v.y() * sim::edm2g4::length,
                                                    v.z() * sim::edm2g4::length,
                                                    v.ctau() / Gaudi::Units::c_light * sim::edm2g4::length);
    const fcc::BareParticle& mccore = mcparticle.core();
    G4PrimaryParticle* part = nullptr;
    if (mcparticle.charge() > 0) {
      part = new G4PrimaryParticle(particleDefPos);
    } else if (mcparticle.charge() < 0) {
      part = new G4PrimaryParticle(particleDefNeg);
    } else {
      part = new G4PrimaryParticle(particleDefNeutral);
    }

    part->SetMass(mccore.p4.mass);
    part->SetCharge(mcparticle.charge());
    double pX = mccore.p4.px * sim::edm2g4::energy;
    double pY = mccore.p4.py * sim::edm2g4::energy;
    double pZ = mccore.p4.pz * sim::edm2g4::energy;
    part->SetUserInformation(new sim::ParticleInformation(mcparticle));
    part->SetMomentum(pX, pY, pZ);
    g4Vertex->SetPrimary(part);
    theEvent->AddPrimaryVertex(g4Vertex);
  }
  return theEvent;
}
