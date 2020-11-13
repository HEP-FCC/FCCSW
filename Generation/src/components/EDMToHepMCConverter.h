#ifndef GENERATION_EDMTOHEPMCCONVERTER_H
#define GENERATION_EDMTOHEPMCCONVERTER_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "HepMC/GenEvent.h"
#include "HepMC/Units.h"

namespace edm4hep {
class MCParticleCollection;
}

class EDMToHepMCConverter : public GaudiAlgorithm {

public:
  /// Constructor.
  EDMToHepMCConverter(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Handle for the HepMC to be read
  DataHandle<HepMC::GenEvent> m_hepmchandle{"hepmc", Gaudi::DataHandle::Writer, this};
  /// Handle for the genparticles to be written
  DataHandle<edm4hep::MCParticleCollection> m_genphandle{"GenParticles", Gaudi::DataHandle::Reader, this};
};

#endif
