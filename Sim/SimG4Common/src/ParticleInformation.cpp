#include "SimG4Common/ParticleInformation.h"

namespace sim {
ParticleInformation::ParticleInformation(const fcc::MCParticle& aMCpart, fcc::Particle& aPart): m_mcpart(aMCpart), m_part(aPart) {}

ParticleInformation::~ParticleInformation() {}

void ParticleInformation::Print() const {}

const fcc::MCParticle& ParticleInformation::GetMCParticle() const {
  return m_mcpart;
}

fcc::Particle& ParticleInformation::GetParticle() {
  return m_part;
}
}
