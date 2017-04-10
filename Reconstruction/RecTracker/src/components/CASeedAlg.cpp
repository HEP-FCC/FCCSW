
#include "DetInterface/IGeoSvc.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDRec/API/IDDecoder.h"
#include "DDSegmentation/BitField64.h"

#include <cmath>

#include "CASeedAlg.h"
#include "datamodel/PositionedTrackHitCollection.h"

// STL
#include <array>
#include <cmath>
#include <iostream>
#include <vector>

#include "CACell.h"
#include "CellularAutomaton.h"
#include "FKDTree.h"
#include "KDDoublets.h"

DECLARE_ALGORITHM_FACTORY(CASeedAlg)

CASeedAlg::CASeedAlg(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("positionedTrackHits", m_positionedTrackHits, "hits/TrackerPositionedHits");
}

CASeedAlg::~CASeedAlg() {}

StatusCode CASeedAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  return sc;
}


struct cellIDsmaller {
  bool operator() (const fcc::PositionedTrackHit& h1, const fcc::PositionedTrackHit& h2) {
    int system1 = h1.cellId() % 32;
    int system2 = h2.cellId() % 32;
    int layer1 = (h1.cellId() >> 5) % 32;
    int layer2 = (h2.cellId() >> 5) % 32;
    if (system1 < system2) return true;
    if (system2 < system1) return false;
    return (layer1 < layer2);
  }
};

struct cellIDequal {
  explicit cellIDequal(unsigned int i): n(i) {}
  bool operator() (const fcc::PositionedTrackHit& h1) {
    return (h1.cellId() % 1024) == n;
  }
private:
  unsigned int n;
};

StatusCode CASeedAlg::execute() {

  const unsigned int numLayers = 3;
  const std::array<std::pair<int, int>, numLayers> seedingLayerIds = {{{0,0}, {0,1}, {0,2}}};

  std::array<std::vector<FKDPoint<float, 3>>, numLayers> layerKDpointVectors;
  std::array<std::vector<fcc::PositionedTrackHit>, numLayers> layerHitVectors;

  const fcc::PositionedTrackHitCollection* hitColl = m_positionedTrackHits.get();

  std::vector<fcc::PositionedTrackHit> hitsToSort;

  for (const auto ptc : *hitColl) {
    hitsToSort.push_back(ptc);
  }
  std::sort(hitsToSort.begin(), hitsToSort.end(), cellIDsmaller());
  
  for (unsigned int layerCounter = 0; layerCounter < numLayers; ++layerCounter) {
    unsigned int hitindex = seedingLayerIds[layerCounter].first + 32 * seedingLayerIds[layerCounter].second; /// @todo remove hardcoded bitfield parameters
    auto it = std::find_if(hitsToSort.begin(), hitsToSort.end(), cellIDequal(hitindex));
    auto it_end = std::find_if_not(it, hitsToSort.end(), cellIDequal(hitindex));
    for(; it !=it_end; ++it) {
      auto pos = (*it).position();
      float t = (*it).time();
      // KDtree with 3 dimensions: phi, z, time
      layerKDpointVectors[layerCounter].push_back(FKDPoint<float, 3>(std::atan2(pos.y, pos.x), pos.z, t, layerKDpointVectors[layerCounter].size()));
      // need to clone hit
      layerHitVectors[layerCounter].push_back((*it).clone());


    }

  }

  debug() << layerHitVectors[0].size() << " hits in first seeding layer." << endmsg;


  std::vector<const HitDoublets*> doubletvector;

  for (unsigned int layerCounter = 1; layerCounter < layerKDpointVectors.size(); ++layerCounter) {
    auto points = layerKDpointVectors[layerCounter];
    auto doublets = new HitDoublets(layerHitVectors[layerCounter - 1], layerHitVectors[layerCounter]);
    debug() << "Build KDTree..." << endmsg;
    FKDTree<float, 3> kdtree(points.size(), points);
    kdtree.build();
    int pointCounter = 0;
    for (auto p : layerKDpointVectors[layerCounter - 1]) {
      auto result_ids = kdtree.search_in_the_box(FKDPoint<float, 3>(p[0] - m_deltaPhi, p[1] - m_deltaZ, p[2] - m_deltaT, 0),
                                                 FKDPoint<float, 3>(p[0] + m_deltaPhi, p[1] + m_deltaZ, p[2] + m_deltaT, 0));

      for (auto r : result_ids) {
        doublets->add(pointCounter, r);
      }
      ++pointCounter;
    }
    debug() << "\tfound " << doublets->size() << " doublets." << endmsg;
    doubletvector.push_back(doublets);
  }

  debug() << "Run Cellular Automaton..." << endmsg;
  auto automaton = new CellularAutomaton<numLayers>();
  automaton->createAndConnectCells(doubletvector, hitColl->size(), 2, 2);  // values for cuts not used at the moment
  automaton->evolve();
  automaton->evolve();
  std::vector<CACell::CAntuplet> theNTuplets;
  automaton->findNtuplets(theNTuplets, 3);
  debug() << "Truth track IDs (if available) : " << endmsg;
  for (auto nt : theNTuplets) {
    debug() << nt[0]->getInnerHit().bits() << "\t" << nt[0]->getOuterHit().bits() << "\t" << nt[1]->getOuterHit().bits() << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode CASeedAlg::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
