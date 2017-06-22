#include "PileupOverlayAlg.h"


DECLARE_ALGORITHM_FACTORY(PileupOverlayAlg)

PileupOverlayAlg::PileupOverlayAlg(const std::string& aName, ISvcLocator* aSvcLoc):
  GaudiAlgorithm(aName, aSvcLoc),
  m_store(),
  m_reader() {
  declareProperty("PileUpTool", m_pileUpTool);
}

StatusCode PileupOverlayAlg::initialize() {

  for (auto& toolname : m_saveToolNames) {
    m_mergeTools.push_back(tool<IEDMMergeTool>(toolname));
  }
  m_minBiasEventIndex = 0;
  StatusCode sc = GaudiAlgorithm::initialize();
  m_reader.openFile(m_pileupFilename);
  m_store.setReader(&m_reader);
  IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = m_flatDist.initialize(randSvc, Rndm::Flat(0., 1.));
  return sc;
}


StatusCode PileupOverlayAlg::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}


StatusCode PileupOverlayAlg::execute() {
  unsigned nEvents = m_reader.getEntries();

  for (auto& tool : m_mergeTools) {
    tool->readSignal();
  }

  const unsigned int numPileUp = m_pileUpTool->numberOfPileUp();
  for(unsigned iev = 0; iev < numPileUp; ++iev) {

    // introduce some randomness into min bias event selection 
    // to avoid bias
    if ( m_randomizePileup == true ) {
      if (m_flatDist() < 0.3) { // skip one in three events
        ++m_minBiasEventIndex;
      }
    }
    m_reader.goToEvent(m_minBiasEventIndex);


  debug() << "Reading in pileup event #" << m_minBiasEventIndex << " ..." << endmsg;
  for (auto& tool : m_mergeTools) {
    tool->readPileupCollection(m_store);
  }

    m_minBiasEventIndex = (m_minBiasEventIndex + 1) % nEvents; // start over from beginning if necessary
   // m_store.clear();
  }

  for (auto& tool : m_mergeTools) {
    tool->mergeCollections();
  }

  return StatusCode::SUCCESS;
}

