#include "PileupMerge.h"


DECLARE_ALGORITHM_FACTORY(PileupMerge)

PileupMerge::PileupMerge(const std::string& aName, ISvcLocator* aSvcLoc):
  GaudiAlgorithm(aName, aSvcLoc),
  m_store(),
  m_reader() {
  declareProperty("pileupGenParticles", m_genParticlesOut, "Generated particles collection (output)");
  declareProperty("pileupGenVertices", m_genVerticesOut, "Generated vertices collection (output)");
  declareProperty("pileupTrackHits", m_trackHitsOut, "Track Hit Collection (output)");
  declareProperty("pileupPosTrackHits", m_posTrackHits, "Positioned Track Hits Collection (output)");
  declareProperty("pileupCaloHits", m_caloHitsOut, "Calo Hit Collection (output)");
  declareProperty("pileupPosCaloHits", m_posCaloHits, "Positioned Calo Hits Collection (output)");
}

StatusCode PileupMerge::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  m_reader.openFile(m_pileupFilename);
  m_store.setReader(&m_reader);
  IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = m_flatDist.initialize(randSvc, Rndm::Flat(0., 1.));
  return sc;
}


StatusCode PileupMerge::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}


StatusCode PileupMerge::execute() {
  unsigned nEvents = m_reader.getEntries();
  unsigned currentEvent = 0;

  const fcc::TrackHitCollection* trackHitCollection;
  fcc::TrackHitCollection* hitCollOut = new fcc::TrackHitCollection();

  const fcc::PositionedTrackHitCollection* posTrackHitCollection;
  fcc::PositionedTrackHitCollection* posHitCollOut = new fcc::PositionedTrackHitCollection();

  const fcc::MCParticleCollection* mcParticleCollection;
  fcc::MCParticleCollection* mcPartCollOut = new fcc::MCParticleCollection();

  const fcc::GenVertexCollection* genVertexCollection;
  fcc::GenVertexCollection* genVertCollOut = new fcc::GenVertexCollection();

  const fcc::CaloHitCollection* caloHitCollection;
  fcc::CaloHitCollection* caloHitCollOut = new fcc::CaloHitCollection();

  const fcc::PositionedCaloHitCollection* posCaloHitCollection;
  fcc::PositionedCaloHitCollection* posCaloHitCollOut = new fcc::PositionedCaloHitCollection();

  for(unsigned iev = 0; iev < m_numPileup; ++iev) {
  
    bool mcParticleCollectionPresent = m_store.get(m_pileupGenParticlesBranchName, mcParticleCollection);
    if (mcParticleCollectionPresent) {
      for (auto elem: (*mcParticleCollection)) {
        mcPartCollOut->push_back(elem.clone());
      }
    }

    bool genVertexCollectionPresent = m_store.get(m_pileupGenVerticesBranchName, genVertexCollection);
    if (genVertexCollectionPresent) {
      for (auto elem: (*genVertexCollection)) {
        genVertCollOut->push_back(elem.clone());
      }
    }

    bool trackHitsPresent = m_store.get(m_pileupTrackHitsBranchName, trackHitCollection);
    if (trackHitsPresent) {
      for (auto elem: (*trackHitCollection)) {
        hitCollOut->push_back(elem.clone());
      }
    }

    bool posTrackHitsPresent = m_store.get(m_pileupPositionedTrackHitsBranchName, posTrackHitCollection);
    if (posTrackHitsPresent) {
      for (auto elem: (*posTrackHitCollection)) {
        posHitCollOut->push_back(elem.clone());
      }
    }

    bool caloHitsPresent = m_store.get(m_pileupCaloHitsBranchName, caloHitCollection);
    if (caloHitsPresent) {
      for (auto elem: (*caloHitCollection)) {
        caloHitCollOut->push_back(elem.clone());
      }
    }

    bool posCaloHitsPresent = m_store.get(m_pileupPositionedCaloHitsBranchName, posCaloHitCollection);
    if (posCaloHitsPresent) {
      for (auto elem: (*posCaloHitCollection)) {
        posCaloHitCollOut->push_back(elem.clone());
      }
    }


    m_store.clear();

    if ( m_randomizePileup == true ) {
      if (m_flatDist() < 0.3) { // skip one in three events
        ++currentEvent;
      }
    }
    currentEvent = (currentEvent + 1) % nEvents; // start over from beginning if necessary
    m_reader.goToEvent(currentEvent);
  }
  m_trackHitsOut.put(hitCollOut);
  return StatusCode::SUCCESS;
}
