#include "PodioMerge.h"


DECLARE_ALGORITHM_FACTORY(PodioMerge)

PodioMerge::PodioMerge(const std::string& aName, ISvcLocator* aSvcLoc):
  GaudiAlgorithm(aName, aSvcLoc) {
  declareInput("trackHitsIn", m_trackHits);
  declareInput("fileHits", m_trackHitVec);
  declareOutput("trackHitsOut", m_trackHitsOut);
}

StatusCode PodioMerge::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  return sc;
}


StatusCode PodioMerge::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}

// takes all the collections in the container described by iterator Iter and copies them to coll
template<typename Iter>
void mergeCollections(Iter coll_vector_it, Iter coll_vector_end, typename Iter::value_type& coll) {
  for (; coll_vector_it!=coll_vector_end; ++coll_vector_it) {
    for (auto elem: (*coll_vector_it)) {
      coll.push_back(elem.clone());
    }
  }
}

StatusCode PodioMerge::execute() {
  const fcc::TrackHitCollection* hitColl = m_trackHits.get();
  fcc::TrackHitCollection* hitCollOut = new fcc::TrackHitCollection();
  for (auto elem: (*hitColl)) {
    hitCollOut->push_back(elem.clone());
  }
  const fcc::TrackHitCollection* hitCollVec = m_trackHitVec.get();
    for (auto elem: (*hitCollVec)) {
      hitCollOut->push_back(elem.clone());
    }
  m_trackHitsOut.put(hitCollOut);
  return StatusCode::SUCCESS;
}
