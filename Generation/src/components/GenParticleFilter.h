#ifndef GENERATION_GENPARTICLEFILTER_H
#define GENERATION_GENPARTICLEFILTER_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "FWCore/DataHandle.h"
#include "datamodel/MCParticleCollection.h"

class GenParticleFilter: public GaudiAlgorithm {
  friend class AlgFactory<GenParticleFilter> ;

public:
  /// Constructor.
  GenParticleFilter(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();
private:
  /// Handle for the ParticleCollection to be read
  DataHandle<fcc::MCParticleCollection> m_igenphandle;
  /// Handle for the genparticles to be written
  DataHandle<fcc::MCParticleCollection> m_ogenphandle;
};

#endif // GENERATION_GENPARTICLEFILTER_H
