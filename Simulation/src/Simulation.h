#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "DataObjects/HepMCEntry.h"
#include "GaudiKernel/DataObjectHandle.h"


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
  DataObjectHandle<HepMCEntry> m_hepmchandle;

};

#endif
