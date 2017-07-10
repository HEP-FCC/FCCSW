#ifndef ClicHCAL_H
#define ClicHCAL_H

#include "papas/detectors/Calorimeter.h"

#include <vector>

namespace papas {

class Particle;
class Cluster;
class Material;
class VolumeCylinder;

/// Clic specific implementation of  HCAL Detector element
///
class ClicHCAL : public Calorimeter {
public:
  /** Constructor
   *
   * @param[in] volume The HCAL cyclinders
   * @param[in] material HCAL material
   * @param[in] eta_crack ask Colin
   * @param[in] emin vector of minimum energy { barrel, endcap}
   * @param[in] eres vector of  vectors of energy resolution { barrel, endcap}
   */
  ClicHCAL(const VolumeCylinder&& volume,
          const Material&& material,
          double m_eta_crack,
          std::vector<std::vector<double>>
              eres,
          std::vector<std::vector<double>>
              eresp);
  /** Constructor
   *
   * @param[in] volume The HCAL cyclinders
   * @param[in] material HCAL material
   * @param[in] eta_crack ask Colin
   * @param[in] emin vector of minimum energy { barrel, endcap}
   * @param[in] eres vector of  vectors of energy resolution { barrel, endcap}
   */
  ClicHCAL(const VolumeCylinder& volume,
          const Material& material,
          double m_etaCrack,
          std::vector<std::vector<double>>
              eres,
          std::vector<std::vector<double>>
              eresp);

  double clusterSize(const Particle& ptc) const override;
  bool acceptance(const Cluster& cluster) const override;
  double energyResolution(double energy, double eta = 0) const override;
  double energyResponse(double energy, double eta = 0) const override;
  // TODOAJR space_resolution(self, ptc):
private:
  double m_etaCrack;
  std::vector<std::vector<double>> m_eres;   /// energy resolution
  std::vector<std::vector<double>> m_eresp;  /// ask Colin what this is};
};

}  // end namespace papas
#endif
