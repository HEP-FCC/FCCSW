#include "PileupOverlayAlg.h"

#include "FWCore/IEDMMergeTool.h"




DECLARE_ALGORITHM_FACTORY(PileupOverlayAlg)

PileupOverlayAlg::PileupOverlayAlg(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc), m_store(), m_reader() {
  declareProperty("PileUpTool", m_pileUpTool);
}

StatusCode PileupOverlayAlg::initialize() {

  for (auto& toolname : m_saveToolNames) {
    m_mergeTools.push_back(tool<IEDMMergeTool>(toolname));
  }
  m_minBiasEventIndex = 0;
  StatusCode sc = GaudiAlgorithm::initialize();
  IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = m_flatDist.initialize(randSvc, Rndm::Flat(0., 1.));

  m_pileupFileIndex = 0;
  if (m_doShuffleInputFiles) {
    m_pileupFileIndex = static_cast<unsigned int>(m_flatDist() * m_pileupFilenames.size());
  }
  m_reader.openFile(m_pileupFilenames[m_pileupFileIndex]);
  m_store.setReader(&m_reader);
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
  for (unsigned iev = 0; iev < numPileUp; ++iev) {

    // introduce some randomness into min bias event selection
    // to avoid bias
    if (m_randomizePileup == true) {
      if (m_flatDist() < m_skipProbability) {
        ++m_minBiasEventIndex;
      }
    }
    m_reader.goToEvent(m_minBiasEventIndex);

    verbose() << "Reading in pileup event #" << m_minBiasEventIndex << " from pool #" << m_pileupFileIndex << " ..."
              << endmsg;
    for (auto& tool : m_mergeTools) {
      tool->readPileupCollection(m_store);
    }

    m_minBiasEventIndex = (m_minBiasEventIndex + 1);
    if (m_minBiasEventIndex >= nEvents) {
      m_minBiasEventIndex = 0;
      if (m_doShuffleInputFiles) {
        m_pileupFileIndex = static_cast<unsigned int>(m_flatDist() * m_pileupFilenames.size());
      } else {
        m_pileupFileIndex = (m_pileupFileIndex + 1) % m_pileupFilenames.size();
      }
      m_store.clearCaches();
      m_reader.closeFile();
      verbose() << "switching to pileup file " << m_pileupFilenames[m_pileupFileIndex] << endmsg;
      m_reader.openFile(m_pileupFilenames[m_pileupFileIndex]);
      nEvents = m_reader.getEntries();
    }
    m_store.clearCaches();
  }

  for (auto& tool : m_mergeTools) {
    tool->mergeCollections();
  }

  return StatusCode::SUCCESS;
}
