#ifndef _HEPMCDUMPER_H_
#define _HEPMCDUMPER_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "HepMC/GenEvent.h"
#include "FWCore/DataHandle.h"

class HepMCDumper: public GaudiAlgorithm {
  friend class AlgFactory<HepMCDumper> ;

public:
  /// Constructor.
  HepMCDumper(const std::string& name, ISvcLocator* svcLoc);
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
