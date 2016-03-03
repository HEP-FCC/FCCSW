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
#include "datamodel/TrackHitCollection.h"
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

 template<typename T>
 StatusCode collFromStoreToDataService( const std::string& collection_name, DataHandle<T>& data_handle);

private:
  bool m_first;
  std::string m_filename;
  std::vector<std::string> m_inputCommands;
  KeepDropSwitch m_switch;
  PodioDataSvc* m_podioDataSvc;
  podio::ROOTReader m_reader;
  podio::EventStore m_store;
  DataHandle<fcc::TrackClusterCollection> m_trackClusters;
  DataHandle<fcc::TrackHitCollection> m_trackHits;
  DataHandle<fcc::TrackClusterHitsAssociationCollection> m_trackClusterHitsAssociations;

};

#endif
