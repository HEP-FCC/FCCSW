#include "PileupTrackHitsMergeTool.h"

#include "podio/EventStore.h" 

#include "datamodel/TrackHitCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"


DECLARE_TOOL_FACTORY(PileupTrackHitsMergeTool)

PileupTrackHitsMergeTool::PileupTrackHitsMergeTool(const std::string& aType, const std::string& aName, const IInterface* aParent):
  GaudiTool(aType, aName, aParent) {
    declareInterface<IEDMMergeTool>(this);
    declareProperty("signalTrackHits", m_TrackHitsIn);
    declareProperty("signalPositionedTrackHits", m_PosTrackHitsIn);
    declareProperty("allTrackHits", m_TrackHitsOut);
    declareProperty("allPositionedTrackHits", m_PosTrackHitsOut);
  }


StatusCode PileupTrackHitsMergeTool::initialize() { return StatusCode::SUCCESS;}

StatusCode PileupTrackHitsMergeTool::finalize() { return StatusCode::SUCCESS;}

StatusCode PileupTrackHitsMergeTool::readPileupCollection(podio::EventStore& store) {
  const fcc::TrackHitCollection* trackHitCollection;
  const fcc::PositionedTrackHitCollection* posTrackHitCollection;

  bool trackHitCollectionPresent = store.get(m_pileupTrackHitsBranchName, trackHitCollection);
  if (trackHitCollectionPresent) {
    m_TrackHitCollections.push_back(trackHitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupTrackHitsBranchName << endmsg;
  }

  bool posTrackHitCollectionPresent = store.get(m_pileupPosTrackHitsBranchName, posTrackHitCollection);
  if (posTrackHitCollectionPresent) {
    m_PosTrackHitCollections.push_back(posTrackHitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupPosTrackHitsBranchName << endmsg;
  }

  
  return StatusCode::SUCCESS;
}

StatusCode PileupTrackHitsMergeTool::readSignal() {
  auto collTrackHitsSig = m_TrackHitsIn.get();
  auto collPosTrackHitsSig = m_PosTrackHitsIn.get();

  m_TrackHitCollections.push_back(collTrackHitsSig);
  m_PosTrackHitCollections.push_back(collPosTrackHitsSig);

  return StatusCode::SUCCESS;

}


StatusCode PileupTrackHitsMergeTool::mergeCollections() {

  debug() << "merge collections ..." << endmsg;

  // ownership given to data service at end of execute
  fcc::TrackHitCollection* collTrackHitsOut = new fcc::TrackHitCollection();
  fcc::PositionedTrackHitCollection* collPosTrackHitsOut = new fcc::PositionedTrackHitCollection();

  for (auto trackHitColl: m_TrackHitCollections) {
    // copy tracker hits
    for (const auto elem : *trackHitColl) {
      collTrackHitsOut->push_back(elem.clone());
    }

  }
  for (auto posTrackHitColl: m_PosTrackHitCollections) {
    // copy positioned track hits
    for (const auto elem : *posTrackHitColl) {
      collPosTrackHitsOut->push_back(elem.clone());
    }
  }

  m_TrackHitsOut.put(collTrackHitsOut);
  m_PosTrackHitsOut.put(collPosTrackHitsOut);

  m_TrackHitCollections.clear();
  m_PosTrackHitCollections.clear();
  return StatusCode::SUCCESS;
}


