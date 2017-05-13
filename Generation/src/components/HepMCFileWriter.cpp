#include "HepMCFileWriter.h"
#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"

DECLARE_COMPONENT(HepMCFileWriter)

HepMCFileWriter::HepMCFileWriter(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hepmc", m_hepmchandle, "The HepMC event to write to text file (input)");
}

StatusCode HepMCFileWriter::initialize() {

  m_file = std::make_unique<HepMC::IO_GenEvent>(m_filename.value().c_str(), std::ios::out);
  // check that readable
  if ((nullptr == m_file) || (m_file->rdstate() == std::ios::failbit)) {
    error() << "Failure to read the file '" + m_filename + "'" << endmsg;
    return StatusCode::FAILURE;
  }
  return GaudiAlgorithm::initialize();
}

StatusCode HepMCFileWriter::execute() {
  const HepMC::GenEvent* theEvent = m_hepmchandle.get();
  m_file->write_event(theEvent);
  return StatusCode::SUCCESS;
}

StatusCode HepMCFileWriter::finalize() {
  m_file.reset();
  return GaudiAlgorithm::finalize();
}
