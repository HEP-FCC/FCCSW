#ifndef EXAMPLES_PODIOINPUT_H
#define EXAMPLES_PODIOINPUT_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"
#include "FWCore/KeepDropSwitch.h"
#include "FWCore/DataHandle.h"

#include "TTree.h"
#include "datamodel/TrackClusterCollection.h"
#include "datamodel/TrackClusterHitsAssociationCollection.h"

#include <vector>

// forward declarations
class TFile;
class PodioDataSvc;

class PodioInput : public GaudiAlgorithm {
  friend class AlgFactory<PodioInput> ;

public:
  /// Constructor.
  PodioInput(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  bool m_first;
  std::string m_filename;
  std::vector<std::string> m_inputCommands;
  KeepDropSwitch m_switch;
  PodioDataSvc* m_podioDataSvc;
  TFile* m_file;
  TTree* m_datatree;
  TTree* m_metadatatree;
  std::vector<podio::CollectionBase*> m_storedCollections;
  podio::ROOTReader m_reader;
  podio::EventStore m_store;
  const fcc::TrackClusterHitsAssociationCollection* clusterHitsAssociations;
  /// Handle for tracker clusters
  DataHandle<fcc::TrackClusterCollection> m_trackClusters;
  /// Handle for tracker hits
  //DataHandle<fcc::TrackHitCollection> m_trackHits;
  /// Handle for tracker hits-clusters associations
  DataHandle<fcc::TrackClusterHitsAssociationCollection> m_trackHitsClusters;

};

#endif
