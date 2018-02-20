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
#include "tricktrack/FKDTree.h"
#include "tricktrack/TTPoint.h"
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
  void createBarrelSpacePoints(std::vector<tricktrack::TTPoint>& thePoints,
                               const fcc::PositionedTrackHitCollection* theHits, std::pair<int, int> sIndex, int trackCutoff);
tricktrack::HitDoublets<Hit>*   findDoublets( std::vector<tricktrack::TTPoint> theInnerHits,  std::vector<tricktrack::TTPoint> theOuterHits);
//void createKDTree( std::vector<Hit>& thePoints, std::pair<int, int> sIndex);
void findDoublets(tricktrack::HitDoublets<Hit>* doublets, std::vector<tricktrack::TTPoint> theInnerHits,  tricktrack::FKDTree<double, 4> theOuterTree, std::vector<tricktrack::TTPoint> theOuterHits);


private:
  /// system and layer ids for the inner barrel layer to be used for seeding
  Gaudi::Property<std::vector<std::pair<int, int>>> m_seedingLayerIndices{this, "seedingLayerIndices", {{0, 0}, {0,1}, {0,2},{0,3},{0,4}}};
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

  /// Parameter for TrickTrack's doublet creation
  Gaudi::Property<double> m_deltaPhi {this, "deltaPhi", 0.3};
  /// Parameter for TrickTrack's doublet creation
  Gaudi::Property<double> m_deltaRho {this, "deltaRho", 1000};
  /// Parameter for TrickTrack's doublet creation
  Gaudi::Property<double> m_deltaZ {this, "deltaZ", 150};
  /// Parameter for TrickTrack's doublet creation
  Gaudi::Property<double> m_deltaT {this, "deltaT", 1050.};

  ToolHandle<ILayerGraphTool> m_layerGraphTool;
  ToolHandle<IDoubletCreationTool> m_doubletCreationTool;
  ToolHandle<IHitFilterTool> m_hitFilterTool;

  std::unique_ptr<tricktrack::HitChainMaker<tricktrack::TTPoint>> m_automaton;
  std::unique_ptr<tricktrack::TrackingRegion> m_trackingRegion;
  tricktrack::CMGraph m_layerGraph;
  std::map<std::pair<int, int>, tricktrack::FKDTree<double, 4>> m_hitTrees;

};

#endif /* RECTRACKER_TRICKTRACKSEEDINGTOOL_H */
