#ifndef RECTRACKER_TRICKTRACKSEEDINGTOOL_H
#define RECTRACKER_TRICKTRACKSEEDINGTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecInterface/IDoubletCreationTool.h"
#include "RecInterface/IHitFilterTool.h"
#include "RecInterface/ILayerGraphTool.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "tricktrack/SpacePoint.h"
#include "tricktrack/CMGraph.h"
#include "tricktrack/TrackingRegion.h"
#include "tricktrack/HitChainMaker.h"

#include <map>





class TrickTrackSeedingTool : public GaudiTool, virtual public ITrackSeedingTool {
public:
  TrickTrackSeedingTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~TrickTrackSeedingTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  virtual std::multimap<unsigned int, unsigned int> findSeeds(const fcc::PositionedTrackHitCollection* theHits) override final;
  void createBarrelSpacePoints(std::vector<tricktrack::SpacePoint<size_t>>& thePoints, std::map<int, unsigned long int>&,
                               const fcc::PositionedTrackHitCollection* theHits, std::pair<int, int> sIndex, int trackCutoff);

private:
  /// system and layer ids for the inner barrel layer to be used for seeding
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices0{this, "seedingLayerIndices0", {0, 0}};
  /// system and layer ids for the middle barrel layer to be used for seeding
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices1{this, "seedingLayerIndices1", {0, 1}};
  /// system and layer ids for the outer barrel layer to be used for seeding
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices2{this, "seedingLayerIndices2", {0, 2}};
  /// system and layer ids for the outer barrel layer to be used for seeding
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices3{this, "seedingLayerIndices3", {0, 3}};
  /// readout used for the barrel seeding layers
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "TrackerBarrelReadout"};
  /// Parameter for TrickTrack's TrackingRegion
  /// coordinate of the center of the luminous region
  Gaudi::Property<double> m_regionOriginX {this, "regionOriginX", 0};
  /// Parameter for TrickTrack's TrackingRegion
  /// coordinate of the center of the luminous region
  Gaudi::Property<double> m_regionOriginY {this, "regionOriginY", 0};
  /// Parameter for TrickTrack's TrackingRegion
  /// radius of the luminous region
  Gaudi::Property<double> m_regionOriginRadius {this, "regionOriginRadius", 0.02};
  /// Parameter for TrickTrack's TrackingRegion
  /// minimum transverse momentum for tracks coming from the luminous region
  Gaudi::Property<double> m_ptMin {this, "ptMin", 0.8};
  /// Parameter for TrickTrack's cell connection
  Gaudi::Property<double> m_thetaCut {this, "thetaCut", 0.002};
  /// Parameter for TrickTrack's cell connection
  Gaudi::Property<double> m_phiCut {this, "phiCut", 0.2};
  /// Parameter for TrickTrack's cell connection
  Gaudi::Property<double> m_hardPtCut {this, "hardPtCut", 0.0};

  ToolHandle<ILayerGraphTool> m_layerGraphTool;
  ToolHandle<IDoubletCreationTool> m_doubletCreationTool;
  ToolHandle<IHitFilterTool> m_hitFilterTool;

  std::unique_ptr<tricktrack::HitChainMaker<tricktrack::SpacePoint<size_t>>> m_automaton;
  std::unique_ptr<tricktrack::TrackingRegion> m_trackingRegion;
  tricktrack::CMGraph m_layerGraph;
};

#endif /* RECTRACKER_TRICKTRACKSEEDINGTOOL_H */
