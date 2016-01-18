#include "SimG4Common/ParticleInformation.h"

namespace sim {
ParticleInformation::ParticleInformation(const MCParticle& aMCpart, Particle& aPart): m_mcpart(aMCpart), m_part(aPart) {}

ParticleInformation::~ParticleInformation() {}

void ParticleInformation::Print() const {}

const MCParticle& ParticleInformation::GetMCParticle() const {
  return m_mcpart;
}

Particle& ParticleInformation::GetParticle() {
  return m_part;
}
}
