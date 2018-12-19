#ifndef SEEDING_KDDOUBLETS_H
#define SEEDING_KDDOUBLETS_H


#include <array>
#include <cmath>
#include <vector>

#include "tricktrack/SpacePoint.h"

namespace tricktrack {

/// @class HitDoublets
/// @author Felice Pantaleo
///
/// @brief Class for a pair of seeding hits in neighboring detector layers.
///
template <typename Hit>
class HitDoublets {
public:
  enum layer { inner = 0, outer = 1 };

  HitDoublets(std::vector<Hit>& innerHits, std::vector<Hit>& outerHits) : layers{{innerHits, outerHits}} {}

  Hit const& hit(int i, layer l) const { return layers[l][indices[2 * i + l]]; }
  float x(int i, layer l) const { return layers[l][indices[2 * i + l]].x(); }
  float y(int i, layer l) const { return layers[l][indices[2 * i + l]].y(); }
  float z(int i, layer l) const { return layers[l][indices[2 * i + l]].z(); }
  float r(int i, layer l) const { return layers[l][indices[2 * i + l]].rho(); }
  float phi(int i, layer l) const { return layers[l][indices[2 * i + l]].phi(); }
  float rv(int i, layer l) const { return layers[l][indices[2 * i + l]].rho(); }
  void add(int innerHit, int outerHit) {
    indices.push_back(innerHit);
    indices.push_back(outerHit);
  }
  size_t size() const { return indices.size() / 2; }
  int innerHitId(int i) const { return indices[2 * i]; }
  int outerHitId(int i) const { return indices[2 * i + 1]; }

private:
  std::array<std::vector<Hit>, 2> layers;
  std::vector<int> indices;
};

}  // end of namespace tricktrack

#endif
