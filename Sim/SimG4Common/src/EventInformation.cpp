#include "SimG4Common/EventInformation.h"

#include "G4Track.hh"

#include "SimG4Common/Units.h"

#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

namespace sim {
EventInformation::EventInformation() : m_genVertices(nullptr), m_mcParticles(nullptr) {}

void EventInformation::setCollections(fcc::GenVertexCollection* genVertices, fcc::MCParticleCollection* mcParticles) {
  m_genVertices = genVertices;
  m_mcParticles = mcParticles;
}

void EventInformation::addParticle(const G4Track* aSecondary) {
  auto edmParticle = m_mcParticles->create();
  auto g4mom = aSecondary->GetMomentum();
  auto g4energy = aSecondary->GetTotalEnergy();
  float mass = g4energy * g4energy - g4mom.mag2();
  if (mass < 0) {
    mass = sqrt(-mass);
  } else {
    mass = sqrt(mass);
  }
  size_t g4ID = aSecondary->GetTrackID();

  edmParticle.p4().px = g4mom.x() * sim::g42edm::energy;
  edmParticle.p4().py = g4mom.y() * sim::g42edm::energy;
  edmParticle.p4().pz = g4mom.z() * sim::g42edm::energy;
  edmParticle.p4().mass = mass * sim::g42edm::energy;
  edmParticle.core().bits = g4ID;
  edmParticle.core().pdgId = aSecondary->GetDynamicParticle()->GetDefinition()->GetPDGEncoding();

  auto g4EndPos = aSecondary->GetPosition();
  auto edmEndVertex = m_genVertices->create();
  edmEndVertex.x(g4EndPos.x() * sim::g42edm::length);
  edmEndVertex.y(g4EndPos.y() * sim::g42edm::length);
  edmEndVertex.z(g4EndPos.z() * sim::g42edm::length);
  edmEndVertex.ctau(aSecondary->GetGlobalTime() * sim::g42edm::length);

  size_t edmVtxId = edmEndVertex.getObjectID().index;
  m_g4IdToEndVertexMap[g4ID] = edmVtxId;

  size_t motherID = aSecondary->GetParentID();
  if (motherID) {  // the particle is not a primary
    auto parentVtxId = m_g4IdToEndVertexMap.find(motherID);
    if (parentVtxId != m_g4IdToEndVertexMap.end()) {
      auto prodVertex = m_genVertices->at(parentVtxId->second);
      edmParticle.startVertex(prodVertex);
    }
  } else {
    auto g4StartPos = aSecondary->GetVertexPosition();
    auto edmStartVertex = m_genVertices->create();
    edmStartVertex.x(g4StartPos.x() * sim::g42edm::length);
    edmStartVertex.y(g4StartPos.y() * sim::g42edm::length);
    edmStartVertex.z(g4StartPos.z() * sim::g42edm::length);
    edmStartVertex.ctau(aSecondary->GetGlobalTime() - aSecondary->GetLocalTime() * sim::g42edm::length);
  }
}
}
