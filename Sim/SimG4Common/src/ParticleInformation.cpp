#include "SimG4Common/ParticleInformation.h"

namespace sim {
ParticleInformation::ParticleInformation(const fcc::MCParticle& aMCpart) : m_mcParticle(aMCpart), m_smeared(false) {}

ParticleInformation::~ParticleInformation() {}

void ParticleInformation::Print() const {}

const fcc::MCParticle& ParticleInformation::mcParticle() const { return m_mcParticle; }
void ParticleInformation::setEndMomentum(const CLHEP::Hep3Vector& aMom) { m_endMomentum = aMom; }
const CLHEP::Hep3Vector& ParticleInformation::endMomentum() const { return m_endMomentum; }
void ParticleInformation::setVertexPosition(const CLHEP::Hep3Vector& aPos) { m_vertexPosition = aPos; }
const CLHEP::Hep3Vector& ParticleInformation::vertexPosition() const { return m_vertexPosition; }
void ParticleInformation::setEndStatus(int aStat) { m_endStatus = aStat; }
int ParticleInformation::endStatus() const { return m_endStatus; }
void ParticleInformation::setSmeared(bool aFlag) { m_smeared = aFlag; }
bool ParticleInformation::smeared() const { return m_smeared; }
}
