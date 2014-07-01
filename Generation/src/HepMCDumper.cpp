#include "HepMCDumper.h"
DECLARE_COMPONENT(HepMCDumper)

HepMCDumper::HepMCDumper(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc)
{
  declareInput("hepmc", m_hepmchandle);
}

StatusCode HepMCDumper::initialize() {
  return GaudiAlgorithm::initialize();
}

StatusCode HepMCDumper::execute() {
  HepMCEntry* theEvent = m_hepmchandle.get();
  theEvent->getEvent()->print();
  return StatusCode::SUCCESS;
}

StatusCode HepMCDumper::finalize() {
  return GaudiAlgorithm::finalize();
}
