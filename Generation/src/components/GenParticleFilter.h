#ifndef GENERATION_GENPARTICLEFILTER_H
#define GENERATION_GENPARTICLEFILTER_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "FWCore/DataHandle.h"

// forward declarations:
namespace fcc {
class MCParticleCollection;
}

/** @class GenParticleFilter Generation/src/components/GenParticleFilter.h GenParticleFilter.h
 *
 *  Creates a new MCParticle collection containing all particles that have one of the accepted statuses.
 *  The accepted statuses are set with property accept, e.g. myfilter = GenParticleFilter(accept=[1,2,3])
 *
 *  @author C. Bernet
 *  @author J. Lingemann
*/

class GenParticleFilter: public GaudiAlgorithm {
  friend class AlgFactory<GenParticleFilter> ;

public:
  /// Constructor.
  GenParticleFilter(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute: Applies the filter
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();
private:
  /// Statuses to accept
  std::vector<unsigned> m_accept;
  /// Handle for the ParticleCollection to be read
  DataHandle<fcc::MCParticleCollection> m_iGenpHandle;
  /// Handle for the genparticles to be written
  DataHandle<fcc::MCParticleCollection> m_oGenpHandle;
};

#endif // GENERATION_GENPARTICLEFILTER_H
