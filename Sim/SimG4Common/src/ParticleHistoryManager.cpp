#include "SimG4Common/ParticleHistoryManager.h"

namespace sim {
ParticleHistoryManager::ParticleHistoryManager() : m_genVertices(nullptr), m_mcParticles(nullptr) {}

void ParticleHistoryManager::setCollections(fcc::GenVertexCollection* genVertices,
                                            fcc::MCParticleCollection* mcParticles) {
  m_genVertices = genVertices;
  m_mcParticles = mcParticles;
}

bool ParticleHistoryManager::isAlreadyTracked(G4Track* aSecondary) {}
void ParticleHistoryManager::addParticle(G4Track* aSecondary) {}
}
