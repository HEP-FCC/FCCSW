#ifndef TRICKTRACK_CMGRAPH_H
#define TRICKTRACK_CMGRAPH_H

#include <array>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace tricktrack {

/** @struct CMLayer
 *
 * struct representing a layer in the detector topology
 */
struct CMLayer {
  CMLayer(const std::string& layerName, std::size_t numberOfHits) : theName(layerName) {
    isOuterHitOfCell.resize(numberOfHits);
  }

  bool operator==(const std::string& otherString) { return otherString == theName; }

  std::string name() const { return theName; }

  std::vector<int> theOuterLayerPairs;
  std::vector<int> theInnerLayerPairs;

  std::vector<int> theOuterLayers;
  std::vector<int> theInnerLayers;
  std::vector<std::vector<unsigned int>> isOuterHitOfCell;

private:
  std::string theName;
};

/** @struct CMLayerPair
 *
 * struct representing a pair of layers in the detector topology.
 */
struct CMLayerPair {

  CMLayerPair(int a, int b)

  {
    theLayers[0] = a;
    theLayers[1] = b;
  }

  bool operator==(const CMLayerPair& otherLayerPair) {
    return (theLayers[0] == otherLayerPair.theLayers[0]) && (theLayers[1] == otherLayerPair.theLayers[1]);
  }

  std::array<int, 2> theLayers;
  std::array<unsigned int, 2> theFoundCells = {{0, 0}};
};

/** struct CMGraph
 *
 * struct representing the full topology of the detector
 */
struct CMGraph {
  std::vector<CMLayer> theLayers;
  std::vector<CMLayerPair> theLayerPairs;
  std::vector<int> theRootLayers;
};

} // namespace tricktrack

#endif /* TRICKTRACK_CMGRAPH_H */
