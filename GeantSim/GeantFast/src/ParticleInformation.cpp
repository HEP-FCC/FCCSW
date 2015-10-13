#include "ParticleInformation.h"

ParticleInformation::ParticleInformation(const MCParticleHandle aMC): m_mcpart(aMC)
{}

ParticleInformation::~ParticleInformation()
{}

void ParticleInformation::Print() const
{}

const MCParticleHandle ParticleInformation::GetMCParticleHandle() const
{
   return m_mcpart;
}
