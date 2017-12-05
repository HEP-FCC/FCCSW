#ifndef TESTGENERATION_HEPMCFROMSCRATCH
#define TESTGENERATION_HEPMCFROMSCRATCH

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"

namespace HepMC {
class GenEvent;
}

class HepMCFromScratch : public GaudiAlgorithm {
  friend class AlgFactory<HepMCFromScratch>;

public:
  HepMCFromScratch(const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode initialize();

  virtual StatusCode execute();

  virtual StatusCode finalize();

private:
  DataHandle<HepMC::GenEvent> m_hepmchandle{"hepmc", Gaudi::DataHandle::Writer, this};
};

#endif
