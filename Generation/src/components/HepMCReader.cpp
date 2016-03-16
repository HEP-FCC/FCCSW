
#include "HepMCReader.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IEventProcessor.h"

DECLARE_COMPONENT(HepMCReader)

HepMCReader::HepMCReader(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc),
  m_filename() {
  declareProperty("Filename", m_filename="",
                  "Name of the HepMC file to read");

  declareProperty("PileUpTool", m_pileUpTool);
  declarePublicTool(m_pileUpTool, "ConstPileUp/PileUpTool");

  declareProperty("VertexSmearingTool", m_vertexSmearingTool);
  declarePublicTool(m_vertexSmearingTool, "FlatSmearVertex/VertexSmearingTool");

  declareProperty("HepMCMergeTool", m_HepMCMergeTool);
  declarePublicTool(m_HepMCMergeTool, "HepMCSimpleMerge/HepMCMergeTool");

  declareProperty("FileReaderSignal", m_signalFileReader);
  declarePrivateTool(m_signalFileReader, "HepMCFileReader/FileReaderSignal");
  declareProperty("FileReaderPileUp", m_pileupFileReader);
  declarePrivateTool(m_pileupFileReader, "HepMCFileReader/FileReaderPileup");

  declareOutput("hepmc", m_hepmchandle);
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
  auto tmpEvent = m_hepmchandle.createAndPut();
  const unsigned int numPileUp = m_pileUpTool->numberOfPileUp();
  std::vector<HepMC::GenEvent> eventVector;
  eventVector.reserve(numPileUp+1);

  StatusCode sc = m_signalFileReader->readNextEvent(*tmpEvent);
  if (StatusCode::SUCCESS != sc) {
    return sc;
  }
  m_vertexSmearingTool->smearVertex(*tmpEvent);
  eventVector.push_back(*tmpEvent);
  for (unsigned int i_pileup=0; i_pileup < numPileUp; ++i_pileup) {
    auto puEvt = HepMC::GenEvent();
    sc = m_pileupFileReader->readNextEvent(puEvt);
    if (StatusCode::SUCCESS != sc) {
      return sc;
    }
    m_vertexSmearingTool->smearVertex(puEvt);
    eventVector.push_back(std::move(puEvt));
  }
  tmpEvent = m_HepMCMergeTool->merge(eventVector);
  return StatusCode::SUCCESS;
}


StatusCode HepMCReader::finalize() {
  return GaudiAlgorithm::finalize();
}
