#ifndef GENERATION_HEPMCCONVERTER_H
#define GENERATION_HEPMCCONVERTER_H


#include "GaudiAlg/GaudiAlgorithm.h"
#include "FWCore/DataHandle.h"
#include "HepMC/Units.h"
#include "HepMC/GenEvent.h"

namespace fcc {
class MCParticleCollection;
class GenVertexCollection;
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
