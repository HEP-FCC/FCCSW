#include "SimG4Common/EventInformation.h"

#include "G4Track.hh"

#include "SimG4Common/Units.h"

#include "edm4hep/MCParticleCollection.h"

namespace sim {
EventInformation::EventInformation() { 
  m_mcParticles = new edm4he[::MCParticleCollection();
}

void EventInformation::setCollections(edm4hep::MCParticleCollection*& aMCParticleCollection) {
  // ownership is transferred here - to SaveTool which is supposed to put it in the event store
  aMCParticleCollection = m_mcParticles;
}

void EventInformation::addParticle(const G4Track* aSecondary) {
  auto edmParticle = m_mcParticles->create();
  auto g4mom = aSecondary->GetMomentum();
  auto g4energy = aSecondary->GetTotalEnergy();
  float mass = g4energy * g4energy - g4mom.mag2();
  mass = sqrt(fabs(mass));
  size_t g4ID = aSecondary->GetTrackID();

  edmParticle.setMomentum({g4mom.x() * sim::g42edm::energy,
        g4mom.y() * sim::g42edm::energy,
        g4mom.z() * sim::g42edm::energy, });
  edmParticle.setMass(mass * sim::g42edm::energy);
  edmParticle.setSimulatorStatus(g4ID);
  edmParticle.setPDG(aSecondary->GetDynamicParticle()->GetDefinition()->GetPDGEncoding());

  auto g4EndPos = aSecondary->GetPosition();
  edmParticle.setEndPoint({
    g4EndPos.x() * sim::g42edm::length,
    g4EndPos.y() * sim::g42edm::length,
    g4EndPos.z() * sim::g42edm::length,
    });
  edmParticle.setTime(aSecondary->GetGlobalTime() * sim::g42edm::length);

  size_t motherID = aSecondary->GetParentID();
  auto g4StartPos = aSecondary->GetVertexPosition();
  edmpParticle.setVertex({
    g4StartPos.x() * sim::g42edm::length),
    g4StartPos.y() * sim::g42edm::length),
    g4StartPos.z() * sim::g42edm::length),
    });
  // todo: no time at endpoint
}

} //namespace sim
