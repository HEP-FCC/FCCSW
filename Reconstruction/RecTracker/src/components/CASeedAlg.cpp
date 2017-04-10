
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

  m_geoSvc = service("GeoSvc");

  StatusCode sc = GaudiAlgorithm::initialize();
  return sc;
}

StatusCode CASeedAlg::execute() {

  const unsigned int numLayers = 3;
  std::array<std::vector<FKDPoint<float, 3>>, numLayers> layerVectors;
  std::array<std::vector<fcc::PositionedTrackHit>, numLayers> layerHitVectors;

  const fcc::PositionedTrackHitCollection* hitColl = m_positionedTrackHits.get();

  for (const auto ptc : *hitColl) {
    // if (ptc.bits() == 1) { // check if primary particle
    int system = ptc.cellId() % 16;                    // hackish, should use ddecoder for this
    unsigned int tmpLayer = (ptc.cellId() >> 4) % 32;  // see above
    unsigned int layer = tmpLayer - 1;
    debug() << system << "\t" << layer << endmsg;
    if ((0 < tmpLayer) && tmpLayer < numLayers + 1 && system == 11) {  /// @todo: remove hardcoded values for system id
      auto pos = ptc.position();
      float t = ptc.time();
      // KDtree with 3 dimensions: phi, z, time
      layerVectors[layer].push_back(FKDPoint<float, 3>(std::atan2(pos.y, pos.x), pos.z, t, layerVectors[layer].size()));
      // need to clone hit
      layerHitVectors[layer].push_back(ptc.clone());
    }

    //}
  }
  debug() << layerHitVectors[2].size() << " hits in outermost layer." << endmsg;


  std::vector<const HitDoublets*> doubletvector;

  // loop over layer pairs
  /// @todo: loop from outside in
  for (unsigned int layerCounter = 1; layerCounter < layerVectors.size(); ++layerCounter) {
    auto points = layerVectors[layerCounter];
    auto doublets = new HitDoublets(layerHitVectors[layerCounter - 1], layerHitVectors[layerCounter]);
    FKDTree<float, 3> kdtree(points.size(), points);
    kdtree.build();
    int pointCounter = 0;
    for (auto p : layerVectors[layerCounter - 1]) {
      auto result_ids = kdtree.search_in_the_box(FKDPoint<float, 3>(p[0] - deltaPhi, p[1] - deltaZ, p[2] - deltaT, 0),
                                                 FKDPoint<float, 3>(p[0] + deltaPhi, p[1] + deltaZ, p[2] + deltaT, 0));

      for (auto r : result_ids) {
        doublets->add(pointCounter, r);
      }
      ++pointCounter;
    }
    doubletvector.push_back(doublets);
  }

  auto automaton = new CellularAutomaton<numLayers>();
  automaton->createAndConnectCells(doubletvector, 40, 2, 2);  // values for cuts not used at the moment
  automaton->evolve();
  automaton->evolve();
  std::vector<CACell::CAntuplet> theNTuplets;
  automaton->findNtuplets(theNTuplets, 3);
  debug() << theNTuplets.size() << endmsg;
  // dump cell positions
  for (auto nt : theNTuplets) {
    debug() << nt[0]->getInnerR() << "\t" << nt[0]->getInnerPhi() << endmsg;
    debug() << nt[0]->getOuterR() << "\t" << nt[0]->getOuterPhi() << endmsg;
    debug() << nt[1]->getInnerR() << "\t" << nt[1]->getInnerPhi() << endmsg;
    debug() << nt[1]->getOuterR() << "\t" << nt[1]->getOuterPhi() << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode CASeedAlg::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
