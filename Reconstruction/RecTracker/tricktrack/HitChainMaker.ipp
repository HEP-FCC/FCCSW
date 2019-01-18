#include "tricktrack/TrackingRegion.h"

#include <cassert>
#include <queue>

namespace tricktrack {

template <typename Hit>
void HitChainMaker<Hit>::createAndConnectCells(std::vector<HitDoublets<Hit>*>& hitDoublets, TripletFilter<Hit> theFilter) {
  // resize cell container
  int tsize = 0;
  for (auto hd : hitDoublets)
    tsize += hd->size();
  allCells.reserve(tsize);

  // get parameters for alignment
  unsigned int cellId = 0;

  // walk the graph to connect cells
  std::vector<bool> alreadyVisitedLayerPairs;
  alreadyVisitedLayerPairs.resize(theLayerGraph.theLayerPairs.size());
  for (auto visited : alreadyVisitedLayerPairs) {
    visited = false;
  }
  // start with "root" layers
  for (int rootVertex : theLayerGraph.theRootLayers) {

    std::queue<int> LayerPairsToVisit;

    // next destination: outer layer pairs of the root layer
    for (int LayerPair : theLayerGraph.theLayers[rootVertex].theOuterLayerPairs) {
      LayerPairsToVisit.push(LayerPair);
    }

    unsigned int numberOfLayerPairsToVisitAtThisDepth =  // @todo: this does not seem to be used
        LayerPairsToVisit.size();

    while (!LayerPairsToVisit.empty()) {
      auto currentLayerPair = LayerPairsToVisit.front();
      auto& currentLayerPairRef = theLayerGraph.theLayerPairs[currentLayerPair];
      auto& currentInnerLayerRef = theLayerGraph.theLayers[currentLayerPairRef.theLayers[0]];
      auto& currentOuterLayerRef = theLayerGraph.theLayers[currentLayerPairRef.theLayers[1]];
      bool allInnerLayerPairsAlreadyVisited{true};

      for (auto innerLayerPair : currentInnerLayerRef.theInnerLayerPairs) {
        allInnerLayerPairsAlreadyVisited &= alreadyVisitedLayerPairs[innerLayerPair];
      }

      if (alreadyVisitedLayerPairs[currentLayerPair] == false && allInnerLayerPairsAlreadyVisited) {

        const HitDoublets<Hit>* doubletLayerPairId = hitDoublets[currentLayerPair];
        auto numberOfDoublets = doubletLayerPairId->size();
        currentLayerPairRef.theFoundCells[0] = cellId;
        currentLayerPairRef.theFoundCells[1] = cellId + numberOfDoublets;
        for (unsigned int i = 0; i < numberOfDoublets; ++i) {
          allCells.emplace_back(doubletLayerPairId, i, doubletLayerPairId->innerHitId(i),
                                doubletLayerPairId->outerHitId(i));

          currentOuterLayerRef.isOuterHitOfCell[doubletLayerPairId->outerHitId(i)].push_back(cellId);

          cellId++;

          auto& neigCells = currentInnerLayerRef.isOuterHitOfCell[doubletLayerPairId->innerHitId(i)];
          allCells.back().checkAlignmentAndTag(allCells, neigCells, theFilter);
        }
        assert(cellId == currentLayerPairRef.theFoundCells[1]);
        for (auto outerLayerPair : currentOuterLayerRef.theOuterLayerPairs) {
          LayerPairsToVisit.push(outerLayerPair);
        }

        alreadyVisitedLayerPairs[currentLayerPair] = true;
      }
      LayerPairsToVisit.pop();
      numberOfLayerPairsToVisitAtThisDepth--;
      if (numberOfLayerPairsToVisitAtThisDepth == 0) {
        numberOfLayerPairsToVisitAtThisDepth = LayerPairsToVisit.size();
      }
    }
  }
}

template <typename Hit>
void HitChainMaker<Hit>::evolve(const unsigned int minHitsPerNtuplet) {
  allStatus.resize(allCells.size());

  unsigned int numberOfIterations = minHitsPerNtuplet - 2;
  // keeping the last iteration for later
  for (unsigned int iteration = 0; iteration < numberOfIterations - 1; ++iteration) {
    for (auto& layerPair : theLayerGraph.theLayerPairs) {
      for (auto i = layerPair.theFoundCells[0]; i < layerPair.theFoundCells[1]; ++i) {
        allCells[i].evolve(i, allStatus);
      }
    }

    for (auto& layerPair : theLayerGraph.theLayerPairs) {
      for (auto i = layerPair.theFoundCells[0]; i < layerPair.theFoundCells[1]; ++i) {
        allStatus[i].updateState();
      }
    }
  }

  // last iteration

  for (int rootLayerId : theLayerGraph.theRootLayers) {
    for (int rootLayerPair : theLayerGraph.theLayers[rootLayerId].theOuterLayerPairs) {
      auto foundCells = theLayerGraph.theLayerPairs[rootLayerPair].theFoundCells;
      for (auto i = foundCells[0]; i < foundCells[1]; ++i) {
        auto& cell = allStatus[i];
        allCells[i].evolve(i, allStatus);
        cell.updateState();
        if (cell.isRootCell(minHitsPerNtuplet - 2)) {
          theRootCells.push_back(i);
        }
      }
    }
  }
}

template <typename Hit>
void HitChainMaker<Hit>::findNtuplets(std::vector<typename CMCell<Hit>::CMntuplet>& foundNtuplets,
                                     const unsigned int minHitsPerNtuplet) {
  typename CMCell<Hit>::CMntuple tmpNtuplet;
  tmpNtuplet.reserve(minHitsPerNtuplet);

  for (auto root_cell : theRootCells) {
    tmpNtuplet.clear();
    tmpNtuplet.push_back(root_cell);
    allCells[root_cell].findNtuplets(allCells, foundNtuplets, tmpNtuplet, minHitsPerNtuplet);
  }
}

template <typename Hit>
void HitChainMaker<Hit>::findTriplets(std::vector<HitDoublets<Hit>*>& hitDoublets,
                                     std::vector<typename CMCell<Hit>::CMntuplet>& foundTriplets, TripletFilter<Hit> theFilter) {
  int tsize = 0;
  for (auto hd : hitDoublets)
    tsize += hd->size();
  allCells.reserve(tsize);

  unsigned int cellId = 0;

  std::vector<bool> alreadyVisitedLayerPairs;
  alreadyVisitedLayerPairs.resize(theLayerGraph.theLayerPairs.size());
  for (auto visited : alreadyVisitedLayerPairs) {
    visited = false;
  }
  for (int rootVertex : theLayerGraph.theRootLayers) {

    std::queue<int> LayerPairsToVisit;

    for (int LayerPair : theLayerGraph.theLayers[rootVertex].theOuterLayerPairs) {
      LayerPairsToVisit.push(LayerPair);
    }

    unsigned int numberOfLayerPairsToVisitAtThisDepth = LayerPairsToVisit.size();

    while (!LayerPairsToVisit.empty()) {
      auto currentLayerPair = LayerPairsToVisit.front();
      auto& currentLayerPairRef = theLayerGraph.theLayerPairs[currentLayerPair];
      auto& currentInnerLayerRef = theLayerGraph.theLayers[currentLayerPairRef.theLayers[0]];
      auto& currentOuterLayerRef = theLayerGraph.theLayers[currentLayerPairRef.theLayers[1]];
      bool allInnerLayerPairsAlreadyVisited{true};

      for (auto innerLayerPair : currentInnerLayerRef.theInnerLayerPairs) {
        allInnerLayerPairsAlreadyVisited &= alreadyVisitedLayerPairs[innerLayerPair];
      }

      if (alreadyVisitedLayerPairs[currentLayerPair] == false && allInnerLayerPairsAlreadyVisited) {
        const HitDoublets<Hit>* doubletLayerPairId = hitDoublets[currentLayerPair];
        auto numberOfDoublets = doubletLayerPairId->size();
        currentLayerPairRef.theFoundCells[0] = cellId;
        currentLayerPairRef.theFoundCells[1] = cellId + numberOfDoublets;
        for (unsigned int i = 0; i < numberOfDoublets; ++i) {
          allCells.emplace_back(doubletLayerPairId, i, doubletLayerPairId->innerHitId(i),
                                doubletLayerPairId->outerHitId(i));

          currentOuterLayerRef.isOuterHitOfCell[doubletLayerPairId->outerHitId(i)].push_back(cellId);

          cellId++;

          auto& neigCells = currentInnerLayerRef.isOuterHitOfCell[doubletLayerPairId->innerHitId(i)];
          allCells.back().checkAlignmentAndPushTriplet(allCells, neigCells, foundTriplets, theFilter);
        }
        assert(cellId == currentLayerPairRef.theFoundCells[1]);

        for (auto outerLayerPair : currentOuterLayerRef.theOuterLayerPairs) {
          LayerPairsToVisit.push(outerLayerPair);
        }

        alreadyVisitedLayerPairs[currentLayerPair] = true;
      }
      LayerPairsToVisit.pop();
      numberOfLayerPairsToVisitAtThisDepth--;
      if (numberOfLayerPairsToVisitAtThisDepth == 0) {
        numberOfLayerPairsToVisitAtThisDepth = LayerPairsToVisit.size();
      }
    }
  }
}
}  // namespace tricktrack
