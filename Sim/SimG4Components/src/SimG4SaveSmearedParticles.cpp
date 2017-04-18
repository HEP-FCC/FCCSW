#include "SimG4SaveSmearedParticles.h"

// FCCSW
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Common/Units.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCParticleAssociationCollection.h"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_TOOL_FACTORY(SimG4SaveSmearedParticles)

SimG4SaveSmearedParticles::SimG4SaveSmearedParticles(const std::string& aType, const std::string& aName,
                                                     const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareProperty("particles", m_particles, "Handle for the particles to be written");
  declareProperty("particlesMCparticles", m_particlesMCparticles,
                  "Handle for the associations between particles and MC particles to be written");
}

SimG4SaveSmearedParticles::~SimG4SaveSmearedParticles() {}

StatusCode SimG4SaveSmearedParticles::initialize() { return GaudiTool::initialize(); }

StatusCode SimG4SaveSmearedParticles::finalize() { return GaudiTool::finalize(); }

StatusCode SimG4SaveSmearedParticles::saveOutput(const G4Event& aEvent) {
  auto particles = m_particles.createAndPut();
  auto associations = m_particlesMCparticles.createAndPut();
  int n_part = 0;
  for (int i = 0; i < aEvent.GetNumberOfPrimaryVertex(); i++) {
    for (int j = 0; j < aEvent.GetPrimaryVertex(i)->GetNumberOfParticle(); j++) {
      const G4PrimaryParticle* g4particle = aEvent.GetPrimaryVertex(i)->GetPrimary(j);
      sim::ParticleInformation* info = dynamic_cast<sim::ParticleInformation*>(g4particle->GetUserInformation());
      if (info->smeared()) {
        const fcc::MCParticle& MCparticle = info->mcParticle();
        fcc::Particle particle = particles->create();
        fcc::ParticleMCParticleAssociation association = associations->create();
        association.rec(particle);
        association.sim(MCparticle);
        fcc::BareParticle& core = particle.core();
        core.pdgId = g4particle->GetPDGcode();
        core.status = 1;  // how it is defined ???? as in HepMC ?
        core.charge = g4particle->GetCharge();
        core.p4.px = info->endMomentum().x() * sim::g42edm::energy;
        core.p4.py = info->endMomentum().y() * sim::g42edm::energy;
        core.p4.pz = info->endMomentum().z() * sim::g42edm::energy;
        core.p4.mass = g4particle->GetMass() * sim::g42edm::energy;
        core.vertex.x = info->vertexPosition().x() * sim::g42edm::length;
        core.vertex.y = info->vertexPosition().y() * sim::g42edm::length;
        core.vertex.z = info->vertexPosition().z() * sim::g42edm::length;
        n_part++;
      }
    }
  }
  debug() << "\t" << n_part << " particles are stored in smeared particles collection" << endmsg;
  return StatusCode::SUCCESS;
}
