#include "HepMCConverter.h"
DECLARE_COMPONENT(HepMCConverter)

HepMCConverter::HepMCConverter(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc)
{
  declareInput("hepmc", m_hepmchandle);
}

StatusCode HepMCConverter::initialize() {
  return GaudiAlgorithm::initialize();
}

StatusCode HepMCConverter::execute() {
  HepMCEntry* theEvent = m_hepmchandle.get();
  const HepMC::GenEvent* event = theEvent->getEvent();
  typedef HepMC::GenEvent::particle_const_iterator Ipart;
  for(Ipart ipart=event->particles_begin(); 
      ipart!=event->particles_end(); ++ipart) {
    const HepMC::GenParticle& ptc = **ipart; 
    if(ptc.status()==1)
      ptc.print();
  }
  return StatusCode::SUCCESS;
}

StatusCode HepMCConverter::finalize() {
  return GaudiAlgorithm::finalize();
}
