/**
 * @file Clic.cc
 * @brief Implementation of the Clic detector
 */
//#include "papas/detectors/ClicTracker.h"
#include "ClicTracker.h"


#include "papas/datatypes/Track.h"
#include "papas/utility/TRandom.h"

#include <cmath>
#include <map>

namespace papas {

ClicTracker::ClicTracker(const VolumeCylinder& volume)
    : Tracker(Layer::kTracker, volume, Material("void", 0, 0)), m_thetaMax(0.08 * M_PI / 180.) {}

ClicTracker::ClicTracker(const VolumeCylinder&& volume)
    : Tracker(Layer::kTracker, volume, Material("void", 0, 0)), m_thetaMax(0.8 * M_PI / 180.) {
    m_resMap = {{90, {8.2e-2, 9.1e-2}},
                {80, {8.2e-4, 9.1e-3}},
                {30, {9.9e-5, 3.8e-3}},
                {20, {3.9e-5, 1.6e-3}},
                {10, {2e-5, 7.2e-4}}
              };
}

bool ClicTracker::acceptance(const Track& track) const {
  double pt = track.p3().Perp();
  double theta = fabs(track.theta());  /// theta = abs(track.theta())

  if (theta < m_thetaMax) {
    if (pt > 0.4)
      return (rootrandom::Random::uniform(0, 1) < 0.95);
    else if (pt > 2)
      return (rootrandom::Random::uniform(0, 1) < 0.99);
  }
  return false;
}

double ClicTracker::sigmaPtOverPt2(double a, double b, double pt) const {
  /// CLIC CDR Eq. 5.1'''
  return sqrt(a * a + pow(b / pt, 2));
}

double ClicTracker::resolution(const Track& track) const {
  // double pt = track.p3().Perp();  // TODO inherited from Colin: depends on the field
  //(void)pt;                       // suppress unused parameter warning
  // return 1.1e-2;                  // updated on 9/16 from 5e-3;

  /*Returns relative resolution on the track momentum
  CLIC CDR, Table 5.3
  */
  double pt = track.p3().Pt();
  // matching the resmap defined above.
  double theta = fabs(track.theta()) * 180 / M_PI;
  for (const auto& v : m_resMap) {  //reverse order
    if (theta < v.first)
      return sigmaPtOverPt2(v.second.first,v.second.second, pt) * pt;
  }
  throw "tracker resolution not found";
  return 0;
}
  
}  // end namespace papas
