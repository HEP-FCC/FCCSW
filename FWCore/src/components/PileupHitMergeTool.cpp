#include "PileupHitMergeTool.h"

#include "podio/EventStore.h"

#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

typedef PileupHitMergeTool<fcc::CaloHitCollection, fcc::PositionedCaloHitCollection> PileupCaloHitMergeTool;
typedef PileupHitMergeTool<fcc::TrackHitCollection, fcc::PositionedTrackHitCollection> PileupTrackHitMergeTool;
DECLARE_TOOL_FACTORY(PileupTrackHitMergeTool)
DECLARE_COMPONENT_WITH_ID(PileupTrackHitMergeTool, "PileupTrackHitMergeTool")
DECLARE_TOOL_FACTORY(PileupCaloHitMergeTool)
DECLARE_COMPONENT_WITH_ID(PileupCaloHitMergeTool, "PileupCaloHitMergeTool")

template <class Hits, class PositionedHits>
PileupHitMergeTool<Hits, PositionedHits>::PileupHitMergeTool(const std::string& aType, const std::string& aName,
                                                             const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IEDMMergeTool>(this);
  declareProperty("signalHits", m_hitsSignal);
  declareProperty("signalPositionedHits", m_posHitsSignal);
  declareProperty("mergedHits", m_hitsMerged);
  declareProperty("mergedPositionedHits", m_posHitsMerged);
}

template <class Hits, class PositionedHits>
StatusCode PileupHitMergeTool<Hits, PositionedHits>::initialize() {
  return StatusCode::SUCCESS;
}

template <class Hits, class PositionedHits>
StatusCode PileupHitMergeTool<Hits, PositionedHits>::finalize() {
  return StatusCode::SUCCESS;
}

template <class Hits, class PositionedHits>
StatusCode PileupHitMergeTool<Hits, PositionedHits>::readPileupCollection(podio::EventStore& store) {
  // local pointers, to be filled by the event store
  const Hits* hitCollection;
  const PositionedHits* posHitCollection;

  // get collection address and store it in container
  bool hitCollectionPresent = store.get(m_pileupHitsBranchName, hitCollection);
  if (hitCollectionPresent) {
    m_hitCollections.push_back(hitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupHitsBranchName << endmsg;
    return StatusCode::FAILURE;
  }

  /// as above, for the positioned collection
  bool posHitCollectionPresent = store.get(m_pileupPosHitsBranchName, posHitCollection);
  if (posHitCollectionPresent) {
    m_posHitCollections.push_back(posHitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupPosHitsBranchName << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

template <class Hits, class PositionedHits>
StatusCode PileupHitMergeTool<Hits, PositionedHits>::readSignal() {
  // get collection from event sture
  auto collHitsSig = m_hitsSignal.get();
  auto collPosHitsSig = m_posHitsSignal.get();

  // store them in internal container
  m_hitCollections.push_back(collHitsSig);
  m_posHitCollections.push_back(collPosHitsSig);

  return StatusCode::SUCCESS;
}

template <class Hits, class PositionedHits>
StatusCode PileupHitMergeTool<Hits, PositionedHits>::mergeCollections() {

  // ownership given to data service at end of execute
  Hits* collHitsMerged = new Hits();
  PositionedHits* collPosHitsMerged = new PositionedHits();

  for (auto hitColl : m_hitCollections) {
    // copy hits
    for (const auto elem : *hitColl) {
      collHitsMerged->push_back(elem.clone());
    }
  }
  for (auto posHitColl : m_posHitCollections) {
    // copy positioned hits
    for (const auto elem : *posHitColl) {
      collPosHitsMerged->push_back(elem.clone());
    }
  }

  m_hitsMerged.put(collHitsMerged);
  m_posHitsMerged.put(collPosHitsMerged);

  m_hitCollections.clear();
  m_posHitCollections.clear();
  return StatusCode::SUCCESS;
}
