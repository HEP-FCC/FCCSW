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
    const fcc::MCParticleCollection* mcparticles = m_genParticles.get();
    info() << mcparticles << endmsg;

    info() << "MCParticle size: " << mcparticles->size() << endmsg;
    int cntr = 0;
    for (const auto& mcpart : *mcparticles) {
      if (10 > cntr++) {
        info() << "vertex x: " << mcpart.StartVertex().Position().X << endmsg;
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode finalize() {
    return GaudiAlgorithm::finalize();
  }

private:
  DataHandle<fcc::MCParticleCollection> m_genParticles;
};
DECLARE_COMPONENT(ReadTestConsumer)
