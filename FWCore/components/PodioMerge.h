#ifndef PODIOMERGE_H
#define PODIOMERGE_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "FWCore/DataHandle.h"
#include "FWCore/DataHandle.h"
#include "datamodel/TrackHitCollection.h"


namespace fcc {
  class TrackClusterCollection;
  class TrackClusterHitsAssociationCollection;
}


class PodioMerge: public GaudiAlgorithm {
public:
  explicit PodioMerge(const std::string&, ISvcLocator*);
  virtual ~PodioMerge();
  virtual StatusCode initialize() final;
  virtual StatusCode execute() final;
  virtual StatusCode finalize() final;
private:
  
  DataHandle<fcc::TrackHitCollection> m_trackHits;
  DataHandle<fcc::TrackHitCollection> m_trackHitsOut;
  //DataHandle<fcc::TrackClusterCollection> m_trackClusters;
  //DataHandle<fcc::TrackClusterHitsAssociationCollection> m_trackHitsClusters;

  DataHandle<fcc::TrackHitCollection> m_trackHitVec;
  //DataHandle<std::vector<fcc::TrackClusterCollection>> m_trackClusters;
  //DataHandle<std::vector<fcc::TrackClusterHitsAssociationCollection>> m_trackHitsClusters;
};

#endif /* PODIOMERGE_H */
