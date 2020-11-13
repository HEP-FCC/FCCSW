#ifndef RECTRACKER_TRICKTRACKSEEDINGTOOL_H
#define RECTRACKER_TRICKTRACKSEEDINGTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecInterface/IHitFilterTool.h"
#include "RecInterface/ILayerGraphTool.h"
#include "edm4hep/SimTrackerHitCollection.h"

#include "tricktrack/SpacePoint.h"
#include "tricktrack/CMGraph.h"
#include "tricktrack/FKDTree.h"
#include "tricktrack/TrackingRegion.h"
#include "tricktrack/HitChainMaker.h"

#include <map>

namespace tricktrack {
  class TTPoint;
}

/** @class TrickTrackSeedingTool
 * Implementation of a track seeding tool using tricktrack's cellular automaton
 */
class TrickTrackSeedingTool : public GaudiTool, virtual public ITrackSeedingTool {
public:
  TrickTrackSeedingTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~TrickTrackSeedingTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  virtual std::multimap<unsigned int, unsigned int> findSeeds(const edm4hep::SimTrackerHitCollection* theHits) override final;
  void createBarrelSpacePoints(std::vector<tricktrack::TTPoint>& thePoints,
                               const edm4hep::SimTrackerHitCollection* theHits);
tricktrack::HitDoublets<tricktrack::TTPoint>*   findDoublets( std::vector<tricktrack::TTPoint> theInnerHits,  std::vector<tricktrack::TTPoint> theOuterHits);
//void createKDTree( std::vector<Hit>& thePoints, std::pair<int, int> sIndex);
void findDoublets(tricktrack::HitDoublets<tricktrack::TTPoint>* doublets, std::vector<tricktrack::TTPoint> theInnerHits,  tricktrack::FKDTree<tricktrack::TTPoint, double, 4> theOuterTree, std::vector<tricktrack::TTPoint> theOuterHits);


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

  Gaudi::Property<bool> m_cleanHits {this, "cleanHits", true};

  int m_goodTracklets = 0;
  int m_totalTracklets = 0;

  ToolHandle<ILayerGraphTool> m_layerGraphTool{"FCChhSeedingGraphTool/FCChhSeedingGraphTool", this};
  ToolHandle<IHitFilterTool> m_hitFilterTool{"FCChhSeedHitFilterTool/FCChhSeedHitFilterTool", this};

  std::unique_ptr<tricktrack::HitChainMaker<tricktrack::TTPoint>> m_automaton;
  std::unique_ptr<tricktrack::TrackingRegion> m_trackingRegion;
  tricktrack::CMGraph m_layerGraph;
  std::map<std::pair<int, int>, tricktrack::FKDTree<tricktrack::TTPoint, double, 4>> m_hitTrees;

};

#endif /* RECTRACKER_TRICKTRACKSEEDINGTOOL_H */
