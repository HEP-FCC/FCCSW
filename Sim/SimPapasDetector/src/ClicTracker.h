#ifndef ClicTRACKER_H
#define ClicTRACKER_H

#include "papas/detectors/Tracker.h"
#include <map>

namespace papas {

class Track;
class VolumeCylinder;

/// Clic specific implementation of the detector Tracker
///
class ClicTracker : public Tracker {
public:
  /** Constructor
   *
   * @param[in] volume The tracker cyclinders
   */
  ClicTracker(const VolumeCylinder&& volume);
  /** Constructor
   *
   * @param[in] volume The tracker cyclinders
   */
  ClicTracker(const VolumeCylinder& volume);
  double resolution(const Track& track) const override;  ///< describes tracker resolution of momentum
  bool acceptance(const Track& track) const override;      ///< determines if a track is detected
  // TODO space_resolution(self, ptc):
private:
  double sigmaPtOverPt2(double a,double b,double pt) const;
  const double m_thetaMax;
  std::map<int, std::pair<double, double>> m_resMap;
};

}  // end namespace papas
#endif
