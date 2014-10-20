#ifndef _HEPMCCONVERTER_H_
#define _HEPMCCONVERTER_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "DataObjects/HepMCEntry.h"
#include "GaudiKernel/DataObjectHandle.h"

class HepMCConverter: public GaudiAlgorithm {
  friend class AlgFactory<HepMCConverter> ;

public:
  /// Constructor.
  HepMCConverter(const std::string& name, ISvcLocator* svcLoc);
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
