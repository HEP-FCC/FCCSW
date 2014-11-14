#ifndef _GenParticleFilter_H_
#define _GenParticleFilter_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "DataObjects/ParticleCollection.h"
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
  DataObjectHandle<ParticleCollection> m_genphandle;
  /// Handle for the ParticleCollection to be written 
  DataObjectHandle<ParticleCollection> m_filterpart;
};

#endif
