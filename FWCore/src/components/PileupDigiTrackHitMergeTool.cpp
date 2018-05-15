#include "PileupDigiTrackHitMergeTool.h"

#include "podio/EventStore.h"

#include "datamodel/DigiTrackHitAssociationCollection.h"
#include "datamodel/MCParticle.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

DECLARE_TOOL_FACTORY(PileupDigiTrackHitMergeTool)
DECLARE_COMPONENT_WITH_ID(PileupDigiTrackHitMergeTool, "PileupDigiTrackHitMergeTool")

PileupDigiTrackHitMergeTool::PileupDigiTrackHitMergeTool(const std::string& aType, const std::string& aName,
                                                         const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IEDMMergeTool>(this);
  declareProperty("signalPositionedHits", m_hitsSignal);
  declareProperty("signalDigiHits", m_posHitsSignal);
  declareProperty("signalParticles", m_particlesSignal);
  declareProperty("mergedPositionedHits", m_hitsMerged);
  declareProperty("mergedDigiHits", m_digiHitsMerged);
  declareProperty("mergedParticles", m_particlesMerged);
}

StatusCode PileupDigiTrackHitMergeTool::initialize() { return StatusCode::SUCCESS; }

StatusCode PileupDigiTrackHitMergeTool::finalize() { return StatusCode::SUCCESS; }

StatusCode PileupDigiTrackHitMergeTool::readPileupCollection(podio::EventStore& store) {
  // local pointers, to be filled by the event store
  const fcc::PositionedTrackHitCollection* hitCollection;
  const fcc::DigiTrackHitAssociationCollection* digiHitCollection;
  const fcc::MCParticleCollection* particleCollection;

  // get collection address and store it in container
  bool hitCollectionPresent = store.get(m_pileupHitsBranchName, hitCollection);
  if (hitCollectionPresent) {
    m_hitCollections.push_back(hitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupHitsBranchName << endmsg;
    return StatusCode::FAILURE;
  }

  /// as above, for the positioned collection
  bool digiHitCollectionPresent = store.get(m_pileupPosHitsBranchName, digiHitCollection);
  if (digiHitCollectionPresent) {
    m_digiTrackHitsCollections.push_back(digiHitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupPosHitsBranchName << endmsg;
    return StatusCode::FAILURE;
  }

  /// as above, for the particle collection (if wanted)
  if (m_mergeParticles) {
    bool particleCollectionPresent = store.get(m_pileupParticleBranchName, particleCollection);
    if (particleCollectionPresent) {
      m_particleCollections.push_back(particleCollection);
    } else {
      warning() << "No collection could be read from branch " << m_pileupParticleBranchName << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode PileupDigiTrackHitMergeTool::readSignal() {
  // get collection from event store
  auto collHitsSig = m_hitsSignal.get();
  auto collPosHitsSig = m_posHitsSignal.get();
  auto collPartSig = m_particlesSignal.get();
  // store them in internal container
  m_hitCollections.push_back(collHitsSig);
  m_digiTrackHitsCollections.push_back(collPosHitsSig);
  // check if particles should be merged
  if (m_mergeParticles) {
    m_particleCollections.push_back(collPartSig);
  }

  return StatusCode::SUCCESS;
}

StatusCode PileupDigiTrackHitMergeTool::mergeCollections() {

  // ownership given to data service at end of execute
  fcc::PositionedTrackHitCollection* collHitsMerged = new fcc::PositionedTrackHitCollection();
  fcc::DigiTrackHitAssociationCollection* collDigiHitsMerged = new fcc::DigiTrackHitAssociationCollection();
  fcc::MCParticleCollection* collParticlesMerged = new fcc::MCParticleCollection();

  unsigned int collectionCounter = 0;
  for (size_t j = 0; j < m_digiTrackHitsCollections.size(); j++) {
    auto offset = collectionCounter * m_trackIDCollectionOffset;
    auto digiHitColl = m_digiTrackHitsCollections.at(j);
    auto hitColl = m_hitCollections.at(j);
    for (int i = 0; i < digiHitColl->size(); i++) {
      auto clon = hitColl->at(i).clone();
      // add pileup vertex counter with an offset
      // i.e. for the signal event, 'bits' is just the trackID taken from geant
      // for the n-th pileup event, 'bits' is the trackID + n * offset
      // offset needs to be big enough to ensure uniqueness of trackID
      if (hitColl->at(i).bits() > m_trackIDCollectionOffset) {
        error() << "Event contains too many tracks to guarantee a unique trackID";
        error() << " The offset width or trackID field size needs to be adjusted!" << endmsg;
        return StatusCode::FAILURE;
      }
      clon.bits(clon.bits() + offset);
      auto newDigiHit = digiHitColl->at(i).clone();
      newDigiHit.hit(clon);
      collHitsMerged->push_back(clon);
      collDigiHitsMerged->push_back(newDigiHit);
    }
    if (m_mergeParticles) {
      auto partColl = m_particleCollections.at(j);
      for (int i = 0; i < partColl->size(); i++) {
        auto clonParticle = partColl->at(i).clone();
        if (partColl->at(i).bits() > m_trackIDCollectionOffset) {
          std::cout << "bits: " << hitColl->at(i).bits() << ", offset: " << m_trackIDCollectionOffset << std::endl;
          error() << "Event contains too many tracks to guarantee a unique trackID";
          error() << " The offset width or trackID field size needs to be adjusted _ particles!" << endmsg;
          return StatusCode::FAILURE;
        }
        clonParticle.bits(clonParticle.bits() + offset);
        collParticlesMerged->push_back(clonParticle);
      }
    }
    ++collectionCounter;
  }
  m_hitsMerged.put(collHitsMerged);
  m_digiHitsMerged.put(collDigiHitsMerged);
  m_particlesMerged.put(collParticlesMerged);

  m_hitCollections.clear();
  m_digiTrackHitsCollections.clear();
  m_particleCollections.clear();

  return StatusCode::SUCCESS;
}
