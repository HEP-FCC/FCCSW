#include "DummySimulation.h"


DECLARE_COMPONENT(DummySimulation)

DummySimulation::DummySimulation(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc) {
  declareInput("genparticles", m_genphandle);
  declareOutput("particles", m_recphandle);
}

StatusCode DummySimulation::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode DummySimulation::execute() {
  const MCParticleCollection* inparticles = m_genphandle.get();
  ParticleCollection* particles = new ParticleCollection();
  for(auto ipart=inparticles->begin(); 
      ipart!=inparticles->end(); ++ipart) {
    const MCParticle ptc = (*ipart).read();
    if(ptc.Core.Status==1) { 
      ParticleHandle outptc = particles->create();
      outptc.mod().Core = ptc.Core; 
    }
  }
  m_recphandle.put(particles);
 
  return StatusCode::SUCCESS;
}

StatusCode DummySimulation::finalize() {
  return GaudiAlgorithm::finalize();
}
