#ifndef _HEPMCDUMPER_H_
#define _HEPMCDUMPER_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "DataObjects/HepMCEntry.h"
#include "GaudiKernel/DataObjectHandle.h"

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
  DataObjectHandle<HepMCEntry> m_hepmchandle;
};

#endif
