#include "SimG4Common/ParticleInformation.h"

namespace sim {
ParticleInformation::ParticleInformation(const MCParticleHandle& aMCpart): m_mcParticleHandle(aMCpart) {}

ParticleInformation::~ParticleInformation() {}

void ParticleInformation::Print() const {}

const MCParticleHandle& ParticleInformation::mcParticleHandle() const {
  return m_mcParticleHandle;
}
void ParticleInformation::setEndMomentum(const CLHEP::Hep3Vector& aMom) {
  m_endMomentum = aMom;
}
const CLHEP::Hep3Vector& ParticleInformation::endMomentum() const {
  return m_endMomentum;
}
void ParticleInformation::setVertexPosition(const CLHEP::Hep3Vector& aPos) {
  m_vertexPosition = aPos;
}
const CLHEP::Hep3Vector& ParticleInformation::vertexPosition() const {
  return m_vertexPosition;
}
void ParticleInformation::setEndStatus(int aStat) {
  m_endStatus = aStat;
}
const int ParticleInformation::endStatus() const {
  return m_endStatus;
}
}
