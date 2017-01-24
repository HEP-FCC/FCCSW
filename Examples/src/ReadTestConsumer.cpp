#include "GaudiAlg/GaudiAlgorithm.h"

#include "FWCore/DataHandle.h"

#include "datamodel/MCParticleCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/GenVertex.h"


class ReadTestConsumer : public GaudiAlgorithm {

public:
  ReadTestConsumer(const std::string& name, ISvcLocator* svcLoc) :
    GaudiAlgorithm(name, svcLoc)
  {
    declareInput("genParticles", m_genParticles, "allGenParticles");
  }

  ~ReadTestConsumer() {};

  StatusCode initialize() {
    return GaudiAlgorithm::initialize();
  }

  StatusCode execute() {
    // Read the input
    const fcc::MCParticleCollection* mcparticles = m_genParticles.get();

    // Does the reading work?
    debug() << mcparticles << endmsg;
    debug() << "MCParticle size: " << mcparticles->size() << endmsg;
    // counter for debug messages below
    int cntr = 0;
    // Loop over all input particles
    for (const auto& mcpart : *mcparticles) {
      if (10 < cntr++) {
        debug() << "vertex x: " << mcpart.startVertex().position().x << endmsg;
      }

    }
    return StatusCode::SUCCESS;
  }

  StatusCode finalize() {
    return GaudiAlgorithm::finalize();
  }

private:
  /// Particles to read
  DataHandle<fcc::MCParticleCollection> m_genParticles;
};
DECLARE_COMPONENT(ReadTestConsumer)
