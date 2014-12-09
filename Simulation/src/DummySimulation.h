#ifndef _DUMMYSIMULATION_H_
#define _DUMMYSIMULATION_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/DataObjectHandle.h"
#include "DataObjects/ParticleCollection.h"
#include "DataObjects/MCParticleCollection.h"


class DummySimulation: public GaudiAlgorithm {
  friend class AlgFactory<DummySimulation> ;

public:
  /// Constructor.
  DummySimulation(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();
private:

  /// Handle for the MCParticleCollection to be read
  DataObjectHandle<MCParticleCollection> m_genphandle;
  /// Handle for the "reconstructed" to be written
  DataObjectHandle<ParticleCollection> m_recphandle;

};

#endif
