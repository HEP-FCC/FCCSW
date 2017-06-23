#include "PileupOverlayAlg.h"

#include "FWCore/IEDMMergeTool.h"
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"


DECLARE_ALGORITHM_FACTORY(PileupOverlayAlg)

PileupOverlayAlg::PileupOverlayAlg(const std::string& aName, ISvcLocator* aSvcLoc):
  GaudiAlgorithm(aName, aSvcLoc)
  {
  declareProperty("PileUpTool", m_pileUpTool);
}

StatusCode PileupOverlayAlg::initialize() {

  for (auto& toolname : m_saveToolNames) {
    m_mergeTools.push_back(tool<IEDMMergeTool>(toolname));
  }
  m_minBiasEventIndex = 0;
  StatusCode sc = GaudiAlgorithm::initialize();
  m_pileupFileIndex = 0;
  for (auto f: m_pileupFilenames) {
    m_readers.push_back(podio::ROOTReader());
    m_stores.push_back(podio::EventStore());
    m_readers[m_pileupFileIndex].openFile(f);
    m_stores[m_pileupFileIndex].setReader(&m_readers[m_pileupFileIndex]);
    ++m_pileupFileIndex;
  }
  m_pileupFileIndex = 0;
  IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = m_flatDist.initialize(randSvc, Rndm::Flat(0., 1.));
  return sc;
}


StatusCode PileupOverlayAlg::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}


StatusCode PileupOverlayAlg::execute() {
  unsigned nEvents = m_readers[m_pileupFileIndex].getEntries();

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
    m_readers[m_pileupFileIndex].goToEvent(m_minBiasEventIndex);


  debug() << "Reading in pileup event #" << m_minBiasEventIndex << " ..." << endmsg;
  for (auto& tool : m_mergeTools) {
    tool->readPileupCollection(m_stores[m_pileupFileIndex]);
  }

    m_minBiasEventIndex = (m_minBiasEventIndex + 1); 
    if (m_minBiasEventIndex >= nEvents) {
      m_minBiasEventIndex = 0;
      m_pileupFileIndex = (m_pileupFileIndex + 1) % m_pileupFilenames.size();
    }
  }

  for (auto& tool : m_mergeTools) {
    tool->mergeCollections();
  }

  return StatusCode::SUCCESS;
}

