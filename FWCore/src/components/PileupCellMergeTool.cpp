#include "PileupCellMergeTool.h"

#include "podio/EventStore.h"

#include "datamodel/CaloHitCollection.h"
#include "datamodel/TrackHitCollection.h"

typedef PileupCellMergeTool<fcc::CaloHitCollection> PileupCaloCellMergeTool;
typedef PileupCellMergeTool<fcc::TrackHitCollection> PileupTrackCellMergeTool;
DECLARE_TOOL_FACTORY(PileupTrackCellMergeTool)
DECLARE_COMPONENT_WITH_ID(PileupTrackCellMergeTool, "PileupTrackCellMergeTool")
DECLARE_TOOL_FACTORY(PileupCaloCellMergeTool)
DECLARE_COMPONENT_WITH_ID(PileupCaloCellMergeTool, "PileupCaloCellMergeTool")

template <class Hits>
PileupCellMergeTool<Hits>::PileupCellMergeTool(const std::string& aType, const std::string& aName,
                                                             const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IEDMMergeTool>(this);
  declareProperty("signalHits", m_hitsSignal);
  declareProperty("mergedHits", m_hitsMerged);
}

template <class Hits>
StatusCode PileupCellMergeTool<Hits>::initialize() {
  return StatusCode::SUCCESS;
}

template <class Hits>
StatusCode PileupCellMergeTool<Hits>::finalize() {
  return StatusCode::SUCCESS;
}

template <class Hits>
StatusCode PileupCellMergeTool<Hits>::readPileupCollection(podio::EventStore& store) {
  // local pointers, to be filled by the event store
  const Hits* hitCollection;

  // get collection address and store it in container
  bool hitCollectionPresent = store.get(m_pileupHitsBranchName, hitCollection);
  if (hitCollectionPresent) {
    m_hitCollections.push_back(hitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupHitsBranchName << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

template <class Hits>
StatusCode PileupCellMergeTool<Hits>::readSignal() {
  // get collection from event sture
  auto collHitsSig = m_hitsSignal.get();

  // store them in internal container
  m_hitCollections.push_back(collHitsSig);

  return StatusCode::SUCCESS;
}

template <class Hits>
StatusCode PileupCellMergeTool<Hits>::mergeCollections() {

  // ownership given to data service at end of execute
  Hits* collHitsMerged = new Hits();

  unsigned int collectionCounter = 0;
  for (auto hitColl : m_hitCollections) {
    // copy hits
    for (const auto elem : *hitColl) {

      auto clon = elem.clone();
      // add pileup vertex counter with an offset
      // i.e. for the signal event, 'bits' is just the trackID taken from geant
      // for the n-th pileup event, 'bits' is the trackID + n * offset
      // offset needs to be big enough to ensure uniqueness of trackID
      if (elem.bits() > m_trackIDCollectionOffset) {
        error() << "Event contains too many tracks to guarantee a unique trackID";
        error() << " The offset width or trackID field size needs to be adjusted!" << endmsg;
        return StatusCode::FAILURE;
      }

      clon.bits(clon.bits() + collectionCounter * m_trackIDCollectionOffset);
      collHitsMerged->push_back(clon);
    }
    ++collectionCounter;
  }

  m_hitsMerged.put(collHitsMerged);

  m_hitCollections.clear();
  return StatusCode::SUCCESS;
}
