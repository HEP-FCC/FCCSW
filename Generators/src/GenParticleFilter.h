#ifndef _GENPARTICLEFILTER_H_
#define _GENPARTICLEFILTER_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "DataObjects/MCParticleCollection.h"
#include "GaudiKernel/DataObjectHandle.h"

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
  DataObjectHandle<MCParticleCollection> m_igenphandle;
  /// Handle for the genparticles to be written
  DataObjectHandle<MCParticleCollection> m_ogenphandle;
};

#endif
