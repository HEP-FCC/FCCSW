#ifndef GENERATION_HEPMCTOEDMCONVERTER_H
#define GENERATION_HEPMCTOEDMCONVERTER_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "HepMC/GenEvent.h"
#include "HepMC/Units.h"

namespace fcc {
class MCParticleCollection;
class GenVertexCollection;
}

class HepMCToEDMConverter : public GaudiAlgorithm {
  friend class AlgFactory<HepMCToEDMConverter>;

public:
  /// Constructor.
  HepMCToEDMConverter(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Handle for the HepMC to be read
  DataHandle<HepMC::GenEvent> m_hepmchandle{"hepmc", Gaudi::DataHandle::Reader, this};
  /// Handle for the genparticles to be written
  DataHandle<fcc::MCParticleCollection> m_genphandle{"genParticles", Gaudi::DataHandle::Writer, this};
  /// Handle for the genvertices to be written
  DataHandle<fcc::GenVertexCollection> m_genvhandle{"genVertices", Gaudi::DataHandle::Writer, this};
};
#endif
