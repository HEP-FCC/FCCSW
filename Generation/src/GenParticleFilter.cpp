#include "GenParticleFilter.h"

#include "DataObjects/LorentzVector.h"

DECLARE_COMPONENT(GenParticleFilter)

GenParticleFilter::GenParticleFilter(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc)
{
  declareInput("hepmc", m_hepmchandle);
  declareOutput("genparticles", m_genphandle);
}

StatusCode GenParticleFilter::initialize() {
  return GaudiAlgorithm::initialize();
}

StatusCode GenParticleFilter::execute() {
  const ParticleCollection* inparticles = m_hepmchandle.get();
  ParticleCollection* particles = new ParticleCollection();
  for(auto ipart=inparticles->begin(); 
      ipart!=inparticles->end(); ++ipart) {
    const ParticleHandle& ptc = *ipart;
    if(ptc.Status()==1) { 
      ParticleHandle& outptc = particles->create();
      outptc.setID(ptc.ID());
      outptc.setStatus(ptc.Status());
      outptc.setP4(ptc.P4());
    }
  }
  m_genphandle.put(particles);
  return StatusCode::SUCCESS;
}

StatusCode GenParticleFilter::finalize() {
  return GaudiAlgorithm::finalize();
}
