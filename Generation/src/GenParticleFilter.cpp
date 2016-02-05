#include "GenParticleFilter.h"

#include "datamodel/LorentzVector.h"

DECLARE_COMPONENT(GenParticleFilter)

GenParticleFilter::GenParticleFilter(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc)
{
  declareInput("genparticles", m_igenphandle);
  declareOutput("genparticles", m_ogenphandle);
}

StatusCode GenParticleFilter::initialize() {
  return GaudiAlgorithm::initialize();
}

StatusCode GenParticleFilter::execute() {
  const fcc::MCParticleCollection* inparticles = m_igenphandle.get();
  fcc::MCParticleCollection* particles = new fcc::MCParticleCollection();
  for(auto ptc : (*inparticles)) {
    if(ptc.Core().Status == 1) {
      fcc::MCParticle outptc = particles->create();
      outptc.Core(ptc.Core()); //COLIN Should not clone only the core!
    }
  }
  m_ogenphandle.put(particles);
  return StatusCode::SUCCESS;
}

StatusCode GenParticleFilter::finalize() {
  return GaudiAlgorithm::finalize();
}
