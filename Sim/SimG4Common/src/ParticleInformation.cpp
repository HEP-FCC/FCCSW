#include "SimG4Common/ParticleInformation.h"

namespace sim {
ParticleInformation::ParticleInformation(const MCParticleHandle& aMCpart, ParticleHandle& aPart): m_mcpart(aMCpart), m_part(aPart) {}

ParticleInformation::~ParticleInformation() {}

void ParticleInformation::Print() const {}

const MCParticleHandle& ParticleInformation::GetMCParticleHandle() const {
  return m_mcpart;
}

ParticleHandle& ParticleInformation::GetParticleHandle() {
  return m_part;
}
}
