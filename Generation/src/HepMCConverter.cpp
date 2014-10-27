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
    if(ptc.status()==1) { 
      // ptc.print();
      ParticleHandle& outptc = particles->create();
      outptc.setID(ptc.pdg_id());
      LorentzVector p4; 
      p4.Pt = ptc.momentum().perp();
      p4.Eta = ptc.momentum().eta();
      p4.Phi = ptc.momentum().phi();
      p4.Mass = ptc.momentum().m();
      outptc.setP4(p4);
    }
  }
  m_genphandle.put(particles);
  return StatusCode::SUCCESS;
}

StatusCode HepMCConverter::finalize() {
  return GaudiAlgorithm::finalize();
}
