#include "HepMCDumper.h"

DECLARE_COMPONENT(HepMCDumper)

HepMCDumper::HepMCDumper(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hepmc", m_hepmchandle, "The HepMC event to dump");
}

StatusCode HepMCDumper::initialize() { return GaudiAlgorithm::initialize(); }

StatusCode HepMCDumper::execute() {
  const HepMC::GenEvent* theEvent = m_hepmchandle.get();
  theEvent->print();
  return StatusCode::SUCCESS;
}

StatusCode HepMCDumper::finalize() { return GaudiAlgorithm::finalize(); }
