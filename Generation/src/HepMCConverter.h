#ifndef _HEPMCCONVERTER_H_
#define _HEPMCCONVERTER_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "FWCore/DataHandle.h"

namespace fcc {
class MCParticleCollection;
class GenVertexCollection;
}

namespace HepMC {
class GenEvent;
}

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
  DataHandle<HepMC::GenEvent> m_hepmchandle;
  /// Handle for the genparticles to be written
  DataHandle<fcc::MCParticleCollection> m_genphandle;
  /// Handle for the genvertices to be written
  DataHandle<fcc::GenVertexCollection> m_genvhandle;
};

#endif
