
#include "HepMCReader.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IEventProcessor.h"

DECLARE_COMPONENT(HepMCReader)

HepMCReader::HepMCReader(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc),
  m_signalFileReader("HepMCFileReader/FileReaderSignal", this),
  m_pileupFileReader("HepMCFileReader/FileReaderPileup", this),
  // FIXME: Why are these public tools?
  m_pileUpTool("ConstPileUp/PileUpTool"),
  m_HepMCMergeTool("HepMCSimpleMerge/HepMCMergeTool"),
  m_vertexSmearingTool("FlatSmearVertex/VertexSmearingTool"),
  m_hepmchandle("HepMC", Gaudi::DataHandle::Writer, this) {

  declareProperty("PileUpTool", m_pileUpTool);
  declareProperty("VertexSmearingTool", m_vertexSmearingTool);
  declareProperty("HepMCMergeTool", m_HepMCMergeTool);
  declareProperty("FileReaderSignal", m_signalFileReader);
  declareProperty("FileReaderPileUp", m_pileupFileReader);
  declareProperty("hepmc", m_hepmchandle);
}

StatusCode HepMCReader::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc.isSuccess()) return sc;
  if ( 0 < m_pileUpTool->getMeanPileUp() ) {
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
  eventVector.reserve(numPileUp+1);

  StatusCode sc = m_signalFileReader->readNextEvent(*theEvent);
  if (StatusCode::SUCCESS != sc) {
    return sc;
  }
  m_vertexSmearingTool->smearVertex(*theEvent);
  for (unsigned int i_pileup=0; i_pileup < numPileUp; ++i_pileup) {
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


StatusCode HepMCReader::finalize() {
  return GaudiAlgorithm::finalize();
}
