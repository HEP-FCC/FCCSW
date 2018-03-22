#include "PileupDigiTrackHitMergeTool.h"

#include "podio/EventStore.h"

#include "datamodel/DigiTrackHitAssociationCollection.h"
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
  declareProperty("mergedPositionedHits", m_hitsMerged);
  declareProperty("mergedDigiHits", m_digiHitsMerged);
}

StatusCode PileupDigiTrackHitMergeTool::initialize() { return StatusCode::SUCCESS; }

StatusCode PileupDigiTrackHitMergeTool::finalize() { return StatusCode::SUCCESS; }

StatusCode PileupDigiTrackHitMergeTool::readPileupCollection(podio::EventStore& store) {
  // local pointers, to be filled by the event store
  const fcc::PositionedTrackHitCollection* hitCollection;
  const fcc::DigiTrackHitAssociationCollection* digiHitCollection;

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
    m_digiTrackHitsCollection.push_back(digiHitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupPosHitsBranchName << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PileupDigiTrackHitMergeTool::readSignal() {
  // get collection from event store
  auto collHitsSig = m_hitsSignal.get();
  auto collPosHitsSig = m_posHitsSignal.get();

  // store them in internal container
  m_hitCollections.push_back(collHitsSig);
  m_digiTrackHitsCollection.push_back(collPosHitsSig);

  return StatusCode::SUCCESS;
}

StatusCode PileupDigiTrackHitMergeTool::mergeCollections() {

  // ownership given to data service at end of execute
  fcc::PositionedTrackHitCollection* collHitsMerged = new fcc::PositionedTrackHitCollection();
  fcc::DigiTrackHitAssociationCollection* collDigiHitsMerged = new fcc::DigiTrackHitAssociationCollection();

  unsigned int collectionCounter = 0;
  for (size_t j = 0; j < m_digiTrackHitsCollection.size(); j++) {
    auto digiHitColl = m_digiTrackHitsCollection.at(j);
    auto hitColl = m_hitCollections.at(j);
    for (size_t i = 0; i < digiHitColl->size(); i++) {
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

      clon.bits(clon.bits() + collectionCounter * m_trackIDCollectionOffset);
      auto newDigiHit = digiHitColl->at(i).clone();
      newDigiHit.hit(clon);
      collHitsMerged->push_back(clon);
      collDigiHitsMerged->push_back(newDigiHit);
    }
    ++collectionCounter;
  }
  m_hitsMerged.put(collHitsMerged);
  m_digiHitsMerged.put(collDigiHitsMerged);

  m_hitCollections.clear();
  m_digiTrackHitsCollection.clear();
  return StatusCode::SUCCESS;
}
