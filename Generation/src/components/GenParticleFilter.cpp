#include "GenParticleFilter.h"

#include "DataObjects/LorentzVector.h"

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
  const MCParticleCollection* inparticles = m_igenphandle.get();
  MCParticleCollection* particles = new MCParticleCollection();
  for(auto ipart=inparticles->begin(); 
      ipart!=inparticles->end(); ++ipart) {
    const MCParticle& ptc = (*ipart).read();
    if(ptc.Core.Status==1) { 
      MCParticleHandle outptc = particles->create();
      outptc.mod().Core = ptc.Core; //COLIN Should not clone only the core!
    }
  }
  m_ogenphandle.put(particles);
  return StatusCode::SUCCESS;
}

StatusCode GenParticleFilter::finalize() {
  return GaudiAlgorithm::finalize();
}
