#ifndef ClicECAL_H
#define ClicECAL_H

#include "papas/detectors/Calorimeter.h"

#include <vector>

namespace papas {

class Particle;
class Cluster;
class Material;
class VolumeCylinder;

/// Clic specific ECAL calorimeter implementation
///
///  It is intended to be replicated/modfied by users to match other required detector characteristics
///
/// ClicECAL inherits from calorimeter class and must implement clusterSize/acceptance/energyResolution etc methods

class ClicECAL : public Calorimeter {
public:
  /** Constructor
   *
   * @param[in] volume The ECAL cyclinders
   * @param[in] material ECAL material
   * @param[in] eta_crack ask Colin
   * @param[in] emin vector of minimum energy { barrel, endcap}
   * @param[in] eres vector of  vectors of energy resolution { barrel, endcap}
   */
  ClicECAL(const VolumeCylinder&& volume, const Material&& material, double eta_crack, std::vector<double> emin,
          std::vector<std::vector<double>> eres, const std::vector<std::vector<double>> eresp);

  /** Minimum size that will be seen by a detector
   @param[in]  ptc  particle that is to be detected
   @return minimum size of cluster that can be seen (TODO units)
   */
  double clusterSize(const Particle& ptc) const override;

  /** Decides whether a cluster will be seen by a detector
   @param[in]  cluster the cluster to be analysed
   @return true if cluster is detected, false it if is too small to be seen
   */
  bool acceptance(const Cluster& cluster) const override;

  /** energy Resolution of ECAL
   @param[in] energy
   @param[in] eta angle of arrival
   @return minimum energy resolution of the detector
   */
  double energyResolution(double energy, double eta = 0) const override;

  /** TODO ask Colin for comment details
   */
  double energyResponse(double energy = 0, double eta = 0) const override;

  // TODOAJR space_resolution(self, ptc):
private:
  double m_etaCrack;                         ///< ask Colin
  std::vector<double> m_emin;                ///< vector contains two elements (Barrel and EndCap)
  std::vector<std::vector<double>> m_eres;   ///< two vectors (Barrel and EndCap) each of 3 elements
                                             ///< TODO describe elements
  std::vector<std::vector<double>> m_eresp;  ///< two vectors (Barrel and EndCap) each of 3 elements
                                             ///< TODO describe elements
};

}  // end namespace papas
#endif
