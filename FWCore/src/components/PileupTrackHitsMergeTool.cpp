#include "PileupTrackHitsMergeTool.h"

#include "podio/EventStore.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

DECLARE_TOOL_FACTORY(PileupTrackHitsMergeTool)

PileupTrackHitsMergeTool::PileupTrackHitsMergeTool(const std::string& aType, const std::string& aName,
                                                   const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IEDMMergeTool>(this);
  declareProperty("signalTrackHits", m_TrackHitsSignal);
  declareProperty("signalPositionedTrackHits", m_PosTrackHitsSignal);
  declareProperty("mergedTrackHits", m_TrackHitsMerged);
  declareProperty("mergedPositionedTrackHits", m_PosTrackHitsMerged);
}

StatusCode PileupTrackHitsMergeTool::initialize() { return StatusCode::SUCCESS; }

StatusCode PileupTrackHitsMergeTool::finalize() { return StatusCode::SUCCESS; }

StatusCode PileupTrackHitsMergeTool::readPileupCollection(podio::EventStore& store) {
  // local pointers, to be filled by the event store
  const fcc::TrackHitCollection* trackHitCollection;
  const fcc::PositionedTrackHitCollection* posTrackHitCollection;

  // get collection address and store it in container
  bool trackHitCollectionPresent = store.get(m_pileupTrackHitsBranchName, trackHitCollection);
  if (trackHitCollectionPresent) {
    m_TrackHitCollections.push_back(trackHitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupTrackHitsBranchName << endmsg;
    return StatusCode::FAILURE;
  }

  /// as above, for the positioned collection
  bool posTrackHitCollectionPresent = store.get(m_pileupPosTrackHitsBranchName, posTrackHitCollection);
  if (posTrackHitCollectionPresent) {
    m_PosTrackHitCollections.push_back(posTrackHitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupPosTrackHitsBranchName << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PileupTrackHitsMergeTool::readSignal() {
  // get collection from event sture
  auto collTrackHitsSig = m_TrackHitsSignal.get();
  auto collPosTrackHitsSig = m_PosTrackHitsSignal.get();

  // store them in internal container
  m_TrackHitCollections.push_back(collTrackHitsSig);
  m_PosTrackHitCollections.push_back(collPosTrackHitsSig);

  return StatusCode::SUCCESS;
}

StatusCode PileupTrackHitsMergeTool::mergeCollections() {

  debug() << "merge collections ..." << endmsg;

  // ownership given to data service at end of execute
  fcc::TrackHitCollection* collTrackHitsMerged = new fcc::TrackHitCollection();
  fcc::PositionedTrackHitCollection* collPosTrackHitsMerged = new fcc::PositionedTrackHitCollection();

  for (auto trackHitColl : m_TrackHitCollections) {
    // copy tracker hits
    for (const auto elem : *trackHitColl) {
      collTrackHitsMerged->push_back(elem.clone());
    }
  }
  for (auto posTrackHitColl : m_PosTrackHitCollections) {
    // copy positioned track hits
    for (const auto elem : *posTrackHitColl) {
      collPosTrackHitsMerged->push_back(elem.clone());
    }
  }

  m_TrackHitsMerged.put(collTrackHitsMerged);
  m_PosTrackHitsMerged.put(collPosTrackHitsMerged);

  m_TrackHitCollections.clear();
  m_PosTrackHitCollections.clear();
  return StatusCode::SUCCESS;
}
