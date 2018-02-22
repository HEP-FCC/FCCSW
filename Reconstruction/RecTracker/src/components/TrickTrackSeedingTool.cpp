#include "TrickTrackSeedingTool.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"



// TrickTrack headers
#include "tricktrack/HitChainMaker.h"
#include "tricktrack/HitDoublets.h"
#include "tricktrack/SpacePoint.h"
#include "tricktrack/FKDTree.h"
#include "tricktrack/FKDPoint.h"
#include "tricktrack/TTPoint.h"

//using Hit = tricktrack::SpacePoint<size_t>; 
using Hit = tricktrack::TTPoint; 
using namespace tricktrack;



DECLARE_TOOL_FACTORY(TrickTrackSeedingTool)

TrickTrackSeedingTool::TrickTrackSeedingTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ITrackSeedingTool>(this);
  declareProperty("LayerGraphTool", m_layerGraphTool);
  declareProperty("DoubletCreationTool", m_doubletCreationTool);
  declareTool(m_layerGraphTool, "BarrelLayerGraphTool4/LayerGraphTool");
  declareTool(m_doubletCreationTool, "DoubletCreationTool/DoubletCreationTool");
  declareTool(m_hitFilterTool, "FastHitFilterTool/FastHitFilterTool");

}

StatusCode TrickTrackSeedingTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  return sc;
}


tricktrack::HitDoublets<Hit>*   TrickTrackSeedingTool::findDoublets( std::vector<Hit> theInnerHits,  std::vector<Hit> theOuterHits) {

  auto doublets = new tricktrack::HitDoublets<Hit>(theInnerHits, theOuterHits);

  // brute force doublet creation
  for (unsigned int i = 0; i < theInnerHits.size(); ++i) {
    for(unsigned int j = 0; j < theOuterHits.size(); ++j) {
      // @todo: very ad-hoc / hard coded parameters
      if ((std::fmod(std::abs(theOuterHits[j].phi() - theInnerHits[i].phi()), 2* M_PI)) < m_deltaPhi) {
        if (std::abs(theOuterHits[j].z() - theInnerHits[i].z()) < m_deltaZ) {
          doublets->add(i, j);
        }
      }
    }
  }
 return doublets;
}

void TrickTrackSeedingTool::findDoublets(tricktrack::HitDoublets<Hit>* doublets, std::vector<tricktrack::TTPoint> theInnerHits,  tricktrack::FKDTree<double, 4> theOuterTree, std::vector<tricktrack::TTPoint> theOuterHits) {
  // brute force doublet creation
  for (int i = 0; i < theInnerHits.size(); ++i) {
    double currentPhi = theInnerHits[i].phi();
    double currentZ = theInnerHits[i].z();
    double currentRho = theInnerHits[i].rho();
    double currentT = theInnerHits[i].t();
    
    TTPoint minPoint(currentRho - m_deltaRho, currentPhi - m_deltaPhi, currentZ - m_deltaZ, currentT - m_deltaT,0);
    TTPoint maxPoint(currentRho + m_deltaRho, currentPhi + m_deltaPhi,  currentZ + m_deltaZ, currentT + m_deltaT,0);

    std::vector<unsigned int> result;
    theOuterTree.search(minPoint, maxPoint, result);
    for(unsigned int j = 0; j < theOuterHits.size(); ++j) {
      if ((std::fmod(std::abs(theOuterHits[j].phi() - theInnerHits[i].phi()), 2* M_PI)) < m_deltaPhi) {
        if (std::abs(theOuterHits[j].z() - theInnerHits[i].z()) < m_deltaZ) {
          if (std::abs(theOuterHits[j].t() - theInnerHits[i].t()) < m_deltaT) {
            doublets->add(i, j);
          }
        }
      }
    }
  }
}

void TrickTrackSeedingTool::createBarrelSpacePoints(std::vector<Hit>& thePoints,
                                                       const fcc::PositionedTrackHitCollection* theHits,
                                                       std::pair<int, int> sIndex,
                                                       int) {
  size_t hitCounter = 0;
  for (auto hit : *theHits) {
    if (m_hitFilterTool->filter(hit.core())) {
        thePoints.emplace_back(std::sqrt(std::pow(hit.position().x,2) + std::pow(hit.position().y,2)), // hit r
        std::atan2(hit.position().y, hit.position().x), // hit phi 
        hit.position().z, hit.core().time, hitCounter);

      }
        ++hitCounter;
  }
}



std::multimap<unsigned int, unsigned int>
TrickTrackSeedingTool::findSeeds(const fcc::PositionedTrackHitCollection* theHits) {
  // get layer graph from tool and create automaton
  m_layerGraph = m_layerGraphTool->getGraph();
  unsigned int numLayers = m_layerGraph.theLayers.size(); 

  // create tracking region with parameters relevant to seed filters
  m_trackingRegion = std::make_unique<tricktrack::TrackingRegion>(m_regionOriginX, m_regionOriginY, m_regionOriginRadius, m_ptMin);
  debug() << "Create automaton ..." << endmsg;
  m_automaton = std::make_unique<HitChainMaker<Hit>>(m_layerGraph);

  // output, associating trackIds with hitIndices
  std::multimap<unsigned int, unsigned int> theSeeds;
  debug() << "seedmap size: " << theSeeds.size() << endmsg;

  std::vector<std::vector<Hit>> layerPoints;
  std::vector<tricktrack::FKDTree<double, 4>> layerTrees;

  // create vector of hits on each seeding layer
  for (unsigned int layerCounter = 0; layerCounter < numLayers; ++layerCounter) {

    layerPoints.emplace_back();
    layerTrees.emplace_back();
    // set the indices the hit filter uses to select hits
    m_hitFilterTool->setIds(m_seedingLayerIndices[layerCounter].first, m_seedingLayerIndices[layerCounter].second);
    createBarrelSpacePoints(layerPoints.back(), theHits, m_seedingLayerIndices[layerCounter], 0 /* debug parameter, currently unused */);
    debug() << "found " << layerPoints.back().size() << " points on Layer " << endmsg;
    layerTrees.back().build(layerPoints.back());
  }

  std::vector<HitDoublets<Hit>*> doublets;
  // go through layer pairs and create doublets
  for (auto layerPair: m_layerGraph.theLayerPairs) {
    unsigned int innerLayerIndex = layerPair.theLayers[0];
    unsigned int outerLayerIndex = layerPair.theLayers[1];
    auto doubletsOnLayerPair = new tricktrack::HitDoublets<Hit>(layerPoints[innerLayerIndex], layerPoints[outerLayerIndex]);
    findDoublets(doubletsOnLayerPair, layerPoints[innerLayerIndex], layerTrees[outerLayerIndex], layerPoints[outerLayerIndex]);

    doublets.push_back(doubletsOnLayerPair);
    debug() << "found "  << doublets.back()->size() << " doublets on layers  " << innerLayerIndex << " and " << outerLayerIndex  << endmsg;
  }

  debug() << "Create and connect cells ..."  << endmsg;
  m_automaton->createAndConnectCells(doublets, *m_trackingRegion, m_thetaCut, m_phiCut, m_hardPtCut);
  debug() << "... cells connected and created." << endmsg;


  constexpr unsigned int minNumberOfHits = 4;
  // do the heavy lifting 
  m_automaton->evolve(minNumberOfHits);
  std::vector<CMCell<Hit>::CMntuplet> foundTracklets;
  m_automaton->findNtuplets(foundTracklets, minNumberOfHits);

  // go through the found seeds, check for correctness and write them to map
  debug() << "found " << foundTracklets.size()<< " tracklets" << endmsg;
  auto cells = m_automaton->getAllCells();
  int numGoodTracklets = 0; 
  int trackletCounter = 0;
  for (const auto& tracklet : foundTracklets) {
   // local variable for a quick check of track seed correctness,
   // set it to -1 in case the trackIds along the tracklet disagree
   int l_trackId = (*theHits)[cells[tracklet[0]].getInnerHit().identifier()].core().bits;
    for(const auto& t: tracklet) {
      auto l_id = cells[t].getInnerHit().identifier();
      theSeeds.insert(std::pair<unsigned int, unsigned int>(trackletCounter, l_id));
      if (l_trackId != (*theHits)[l_id].core().bits) {
        l_trackId = -1;
      }
    }
    auto l_id4 = cells[tracklet[minNumberOfHits - 2]].getOuterHit().identifier();
    theSeeds.insert(std::pair<unsigned int, unsigned int>(trackletCounter, l_id4));
    if (l_trackId != (*theHits)[l_id4].core().bits) {
       l_trackId = -1;
    }
    if (l_trackId != -1) { // all the trackIds were identical, a correctly identified track
      ++numGoodTracklets;
      debug() << "found good track with ID " << l_trackId << endmsg;
    }
    ++trackletCounter;
  }
  debug() << "found " << numGoodTracklets << " correct tracklets" << endmsg;


  return theSeeds;
}

StatusCode TrickTrackSeedingTool::finalize() { return GaudiTool::finalize(); }
