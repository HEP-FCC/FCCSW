#ifndef GENERATION_HEPMCDUMPER_H
#define GENERATION_HEPMCDUMPER_H

#include "GaudiAlg/GaudiAlgorithm.h"

#include "FWCore/DataHandle.h"

#include "HepMC/GenEvent.h"

class HepMCDumper : public GaudiAlgorithm {
  //friend class AlgFactory<HepMCDumper>;

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
  DataHandle<HepMC::GenEvent> m_hepmchandle{"HepMC", Gaudi::DataHandle::Reader, this};
};

#endif  // GENERATION_HEPMCDUMPER_H
