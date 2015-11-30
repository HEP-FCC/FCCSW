#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "HepMC/GenEvent.h"
#include "FWCore/DataHandle.h"


class Simulation: public GaudiAlgorithm {
  friend class AlgFactory<Simulation> ;

public:
  /// Constructor.
  Simulation(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();
private:
  /// Handle for the HepMC to be read
  DataHandle<HepMC::GenEvent> m_hepmchandle;

};

#endif
