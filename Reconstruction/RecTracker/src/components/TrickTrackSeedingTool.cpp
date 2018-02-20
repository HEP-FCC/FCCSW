#include "TrickTrackSeedingTool.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"



// TrickTrack headers
#include "tricktrack/HitChainMaker.h"
#include "tricktrack/HitDoublets.h"
#include "tricktrack/SpacePoint.h"

using Hit = tricktrack::SpacePoint<size_t>; 
using namespace tricktrack;



DECLARE_TOOL_FACTORY(TrickTrackSeedingTool)

TrickTrackSeedingTool::TrickTrackSeedingTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ITrackSeedingTool>(this);
  declareProperty("LayerGraphTool", m_layerGraphTool);
  declareProperty("DoubletCreationTool", m_doubletCreationTool);
  declareTool(m_layerGraphTool, "BarrelLayerGraphTool/LayerGraphTool");
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

void TrickTrackSeedingTool::createBarrelSpacePoints(std::vector<Hit>& thePoints,
                                                    std::map<int, unsigned long int>& indexToTrackId,
                                                       const fcc::PositionedTrackHitCollection* theHits,
                                                       std::pair<int, int> sIndex,
                                                       int) {
  size_t hitCounter = 0;
  for (auto hit : *theHits) {
    if (m_hitFilterTool->filter(hit.core())) {
        thePoints.emplace_back(hit.position().x, hit.position().y, hit.position().z, hitCounter);

        indexToTrackId.insert(std::pair<int, unsigned long int>(hitCounter, hit.core().bits));
      }
        ++hitCounter;
  }
}

std::multimap<unsigned int, unsigned int>
TrickTrackSeedingTool::findSeeds(const fcc::PositionedTrackHitCollection* theHits) {
  // get layer graph from tool and create automaton
  m_layerGraph = m_layerGraphTool->getGraph();

  m_trackingRegion = std::make_unique<tricktrack::TrackingRegion>(m_regionOriginX, m_regionOriginY, m_regionOriginRadius, m_ptMin);
  debug() << "Create automaton ..." << endmsg;
  m_automaton = std::make_unique<HitChainMaker<Hit>>(m_layerGraph);

  std::multimap<unsigned int, unsigned int> theSeeds;
  debug() << "seedmap size: " << theSeeds.size() << endmsg;

  unsigned int numLayers = m_layerGraph.theLayers.size(); 
  std::vector<std::map<int, unsigned long int>> mapLayers; // temp solution to keep track of hit indices
  std::vector<std::vector<Hit>> layerPoints;
  std::vector<std::pair<int,int>> layerIndices {
    m_seedingLayerIndices0,
    m_seedingLayerIndices1,
    m_seedingLayerIndices2,
    m_seedingLayerIndices3};

  for (unsigned int layerCounter = 0; layerCounter < numLayers; ++layerCounter) {

  mapLayers.emplace_back();
  layerPoints.emplace_back();

  m_hitFilterTool->setIds(layerIndices[layerCounter].first, layerIndices[layerCounter].second);
  createBarrelSpacePoints(layerPoints.back(), mapLayers.back(), theHits, layerIndices[layerCounter], 0);

  debug() << "found " << layerPoints.back().size() << " points on Layer " << endmsg;
  }

  std::vector<HitDoublets<Hit>*> doublets;
  for (auto layerPair: m_layerGraph.theLayerPairs) {
    doublets.push_back(m_doubletCreationTool->findDoublets(layerPoints[layerPair.theLayers[0]], layerPoints[layerPair.theLayers[1]]));
    debug() << "found "  << doublets.back()->size() << " doublets on layers  " << layerPair.theLayers[0] << " and " << layerPair.theLayers[1]  << endmsg;
  }

  debug() << "Create and connect cells ..." << doublets.size() << endmsg;
  m_automaton->createAndConnectCells(doublets, *m_trackingRegion, m_thetaCut, m_phiCut, m_hardPtCut);
  debug() << "... cells connected and created." << endmsg;


  constexpr unsigned int minNumberOfHits = 4;

  m_automaton->evolve(minNumberOfHits);
  std::vector<CMCell<Hit>::CMntuplet> foundTracklets;
  m_automaton->findNtuplets(foundTracklets, minNumberOfHits);

  debug() << "found " << foundTracklets.size()<< " tracklets" << endmsg;
  auto cells = m_automaton->getAllCells();
  int numGoodTracklets = 0;
  int trackletCounter = 0;

  for (const auto& tracklet : foundTracklets) {
    auto l_id1 = cells[tracklet[0]].getInnerHit().identifier();
    auto l_id2 = cells[tracklet[1]].getInnerHit().identifier();
    auto l_id3 = cells[tracklet[2]].getInnerHit().identifier();
    auto l_id4 = cells[tracklet[2]].getOuterHit().identifier();
    theSeeds.insert(std::pair<unsigned int, unsigned int>(trackletCounter, l_id1));
    theSeeds.insert(std::pair<unsigned int, unsigned int>(trackletCounter, l_id2));
    theSeeds.insert(std::pair<unsigned int, unsigned int>(trackletCounter, l_id3));
    theSeeds.insert(std::pair<unsigned int, unsigned int>(trackletCounter, l_id4));

    if ((mapLayers[0][l_id1] == mapLayers[1][l_id2]) && 
        (mapLayers[2][l_id3] == mapLayers[1][l_id2])  && mapLayers[3][l_id4] == mapLayers[2][l_id3] ) {
      ++numGoodTracklets;
    }
    ++trackletCounter;
  }
  debug() << "found " << numGoodTracklets << " correct tracklets" << endmsg;


  return theSeeds;
}

StatusCode TrickTrackSeedingTool::finalize() { return GaudiTool::finalize(); }
