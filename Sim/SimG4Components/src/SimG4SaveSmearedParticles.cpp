#include "SimG4SaveSmearedParticles.h"

// FCCSW
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Common/Units.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/MCRecoParticleAssociationCollection.h"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_COMPONENT(SimG4SaveSmearedParticles)

SimG4SaveSmearedParticles::SimG4SaveSmearedParticles(const std::string& aType, const std::string& aName,
                                                     const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareProperty("RecParticles", m_particles, "Handle for the particles to be written");
  declareProperty("MCRecoParticleAssoc", m_particlesMCparticles,
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
        const edm4hep::MCParticle& MCparticle = info->mcParticle();
        edm4hep::Particle particle = particles->create();
        edm4hep::MCRecoParticleAssociation association = associations->create();
        association.setRec(particle);
        association.setSim(MCparticle);
        particle.setPDG(g4particle->GetPDGcode());
        particle.setGeneratorStatus(1);
        particle.setCharge(g4particle->GetCharge());
        particle.setMomentum({
          info->endMomentum().x() * sim::g42edm::energy,
          info->endMomentum().y() * sim::g42edm::energy,
          info->endMomentum().z() * sim::g42edm::energy,
        });
        particle.setMass(g4particle->GetMass() * sim::g42edm::energy);
        particle.setVertex({
            info->vertexPosition().x() * sim::g42edm::length;
            info->vertexPosition().y() * sim::g42edm::length;
            info->vertexPosition().z() * sim::g42edm::length;
        });
        n_part++;
      }
    }
  }
  debug() << "\t" << n_part << " particles are stored in smeared particles collection" << endmsg;
  return StatusCode::SUCCESS;
}
