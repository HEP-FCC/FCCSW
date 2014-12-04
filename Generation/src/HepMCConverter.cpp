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
  HepMCEntry* theEvent = m_hepmchandle.get();
  const HepMC::GenEvent* event = theEvent->getEvent();
  ParticleCollection* particles = new ParticleCollection();
  typedef HepMC::GenEvent::particle_const_iterator Ipart;
  for(Ipart ipart=event->particles_begin(); 
      ipart!=event->particles_end(); ++ipart) {
    const HepMC::GenParticle& ptc = **ipart; 
    // if(ptc.status()==1) { 
    // ptc.print();
    ParticleHandle& outptc = particles->create();
    BareParticle core;
    core.Type = ptc.pdg_id();
    core.Status = ptc.status(); 
    core.P4.Pt = ptc.momentum().perp();
    core.P4.Eta = ptc.momentum().eta();
    core.P4.Phi = ptc.momentum().phi();
    core.P4.Mass = ptc.momentum().m();
    outptc.setCore(core);
  }
  m_genphandle.put(particles);
  return StatusCode::SUCCESS;
}

StatusCode HepMCConverter::finalize() {
  return GaudiAlgorithm::finalize();
}
