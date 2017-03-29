
#include "HepMCReader.h"

#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

DECLARE_COMPONENT(HepMCReader)

HepMCReader::HepMCReader(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("PileUpTool", m_pileUpTool, "Tool that provides the number of pile up events");
  declareProperty("VertexSmearingTool", m_vertexSmearingTool, "Tool that allows to smear vertices");
  declareProperty("HepMCMergeTool", m_HepMCMergeTool, "Tool that merges the pile up events into the signal event");
  declareProperty("FileReaderSignal", m_signalFileReader, "Tool that allows to read signal file events");
  declareProperty("FileReaderPileUp", m_pileupFileReader, "Tool that allows to file containing pile up events");
  declareProperty("hepmc", m_hepmchandle, "The HepMc Event output (output)");
}

StatusCode HepMCReader::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc.isSuccess()) return sc;
  if (0 < m_pileUpTool->getMeanPileUp()) {
    sc = m_pileupFileReader->open(m_pileUpTool->getFilename());
    if (!sc.isSuccess()) return sc;
  }
  sc = m_signalFileReader->open(m_filename);
  if (!sc.isSuccess()) return sc;
  return sc;
}

StatusCode HepMCReader::execute() {
  auto theEvent = m_hepmchandle.createAndPut();
  const unsigned int numPileUp = m_pileUpTool->numberOfPileUp();
  std::vector<HepMC::GenEvent> eventVector;
  eventVector.reserve(numPileUp + 1);

  StatusCode sc = m_signalFileReader->readNextEvent(*theEvent);
  if (StatusCode::SUCCESS != sc) {
    return sc;
  }
  m_vertexSmearingTool->smearVertex(*theEvent);
  for (unsigned int i_pileup = 0; i_pileup < numPileUp; ++i_pileup) {
    auto puEvt = HepMC::GenEvent();
    sc = m_pileupFileReader->readNextEvent(puEvt);
    if (StatusCode::SUCCESS != sc) {
      return sc;
    }
    m_vertexSmearingTool->smearVertex(puEvt);
    eventVector.push_back(std::move(puEvt));
  }
  return m_HepMCMergeTool->merge(*theEvent, eventVector);
}

StatusCode HepMCReader::finalize() { return GaudiAlgorithm::finalize(); }
