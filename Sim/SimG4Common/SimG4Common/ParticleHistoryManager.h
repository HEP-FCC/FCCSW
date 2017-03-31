#ifndef SIMG4COMMON_PARTICLEHISTORYMANAGER_H
#define SIMG4COMMON_PARTICLEHISTORYMANAGER_H

/** @class sim::ParticleHistoryManager SimG4Common/SimG4Common/ParticleHistoryManager.h ParticleHistoryManager.h
 *
 *  Manages history of Monte carlo Particles in simulation
 *
 *  @author J. Lingemann
 */

namespace sim {
class ParticleHistoryManager {
public:
  /// Default constructor
  ParticleHistoryManager();
  /// Destructor
  virtual ~ParticleHistoryManager() = default;

  /** Set the collection pointers to valid collections managed by a tool / algorithm <b>has to be called each event</b>
    * @param genVertices: the vertex collection
    * @param mcParticles: the Monte Carlo particle collection
    */
  void setCollections(fcc::GenVertexCollection* genVertices, fcc::MCParticleCollection* mcParticles);

  bool isAlreadyTracked(G4Track* aSecondary);
  void addParticle(G4Track* aSecondary);

private:
  /// Pointer to the vertex collection, ownership should be handled in a algorithm / tool
  fcc::GenVertexCollection* m_genVertices;
  /// Pointer to the particle collection, ownership should be handled in a algorithm / tool
  fcc::MCParticleCollection* m_mcParticles;
};
}
#endif /* define SIMG4COMMON_PARTICLEHISTORYMANAGER_H */
