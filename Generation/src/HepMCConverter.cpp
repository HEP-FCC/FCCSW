#include "HepMCConverter.h"

#include "DataObjects/LorentzVector.h"

DECLARE_COMPONENT(HepMCConverter)

HepMCConverter::HepMCConverter(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc)
{
  declareInput("hepmc", m_hepmchandle);
  declareOutput("genparticles", m_genphandle);
}

StatusCode HepMCConverter::initialize() {
  return GaudiAlgorithm::initialize();
}

StatusCode HepMCConverter::execute() {
  const HepMC::GenEvent* event = m_hepmchandle.get();
  MCParticleCollection* particles = new MCParticleCollection();
  typedef HepMC::GenEvent::particle_const_iterator Ipart;
  for(Ipart ipart=event->particles_begin(); 
      ipart!=event->particles_end(); ++ipart) {
    const HepMC::GenParticle& ptc = **ipart; 
    // if(ptc.status()==1) { 
    // ptc.print();
    MCParticleHandle outptc = particles->create();
    BareParticle& core = outptc.mod().Core;
    core.Type = ptc.pdg_id();
    core.Status = ptc.status(); 
    core.P4.Px = ptc.momentum().px();
    core.P4.Py = ptc.momentum().py();
    core.P4.Pz = ptc.momentum().pz();
    core.P4.Mass = ptc.momentum().m();
  }
  m_genphandle.put(particles);
  return StatusCode::SUCCESS;
}

StatusCode HepMCConverter::finalize() {
  return GaudiAlgorithm::finalize();
}
