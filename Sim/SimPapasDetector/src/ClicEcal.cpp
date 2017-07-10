/**
 * @file ClicEcal.cc
 * @brief Implementation of the Clic ECAL  */
#include "ClicEcal.h"
//#include "papas/detectors/ClicEcal.h"

#include "papas/datatypes/Cluster.h"
#include "papas/datatypes/Particle.h"

namespace papas {

ClicECAL::ClicECAL(const VolumeCylinder&& volume, const Material&& material, double eta_crack, std::vector<double> emin,
                 const std::vector<std::vector<double>> eres, const std::vector<std::vector<double>> eresp)
    : Calorimeter(Layer::kEcal, volume, material), m_etaCrack(eta_crack), m_emin(emin), m_eres(eres), m_eresp(eresp) {
      m_etaCrack =volume.inner().etaJunction();
    }

double ClicECAL::clusterSize(const Particle& ptc) const {

  int pdgid = abs(ptc.pdgId());
  if ((pdgid == 22) | (pdgid == 11))
    return 0.015;
  else
    return 0.045;
}

bool ClicECAL::acceptance(const Cluster& cluster) const {
  double energy = cluster.energy();
  double eta = fabs(cluster.eta());
  if (eta < m_etaCrack)
    return (energy > m_emin[kBarrel]);
  else if (eta < 2.76)
    return (energy > m_emin[kEndCap]);
  else
    return false;
}
  
  
double ClicECAL::energyResolution(double energy, double eta) const {
  int location = kBarrel;
  double stoch = m_eres[location][0] / sqrt(energy);
  double noise = m_eres[location][1] / energy;
  double constant = m_eres[location][2];
  return sqrt(pow(stoch, 2) + pow(noise, 2) + pow(constant, 2));
}

double ClicECAL::energyResponse(double energy, double eta) const {
    return 1;
}

}  // end namespace papas
