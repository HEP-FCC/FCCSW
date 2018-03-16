#include "PileupClusterMergeTool.h"

#include "podio/EventStore.h"

#include "datamodel/CaloClusterCollection.h"
#include "datamodel/TrackClusterCollection.h"

typedef PileupClusterMergeTool<fcc::CaloClusterCollection> PileupCaloClusterMergeTool;
typedef PileupClusterMergeTool<fcc::TrackClusterCollection> PileupTrackClusterMergeTool;
DECLARE_TOOL_FACTORY(PileupTrackClusterMergeTool)
DECLARE_COMPONENT_WITH_ID(PileupTrackClusterMergeTool, "PileupTrackClusterMergeTool")
DECLARE_TOOL_FACTORY(PileupCaloClusterMergeTool)
DECLARE_COMPONENT_WITH_ID(PileupCaloClusterMergeTool, "PileupCaloClusterMergeTool")

template <class Clusters>
PileupClusterMergeTool<Clusters>::PileupClusterMergeTool(const std::string& aType, const std::string& aName,
                                                             const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IEDMMergeTool>(this);
  declareProperty("signalClusters", m_hitsSignal);
  declareProperty("mergedClusters", m_hitsMerged);
}

template <class Clusters>
StatusCode PileupClusterMergeTool<Clusters>::initialize() {
  return StatusCode::SUCCESS;
}

template <class Clusters>
StatusCode PileupClusterMergeTool<Clusters>::finalize() {
  return StatusCode::SUCCESS;
}

template <class Clusters>
StatusCode PileupClusterMergeTool<Clusters>::readPileupCollection(podio::EventStore& store) {
  // local pointers, to be filled by the event store
  const Clusters* hitCollection;

  // get collection address and store it in container
  bool hitCollectionPresent = store.get(m_pileupClustersBranchName, hitCollection);
  if (hitCollectionPresent) {
    m_hitCollections.push_back(hitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupClustersBranchName << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

template <class Clusters>
StatusCode PileupClusterMergeTool<Clusters>::readSignal() {
  // get collection from event sture
  auto collClustersSig = m_hitsSignal.get();

  // store them in internal container
  m_hitCollections.push_back(collClustersSig);

  return StatusCode::SUCCESS;
}

template <class Clusters>
StatusCode PileupClusterMergeTool<Clusters>::mergeCollections() {

  // ownership given to data service at end of execute
  Clusters* collClustersMerged = new Clusters();

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
      collClustersMerged->push_back(clon);
    }
    ++collectionCounter;
  }

  m_hitsMerged.put(collClustersMerged);

  m_hitCollections.clear();
  return StatusCode::SUCCESS;
}
