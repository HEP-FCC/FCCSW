/**
 * @file Clic.cc
 * @brief Implementation of the Clic detector
 */
#include "ClicHcal.h"

#include "papas/datatypes/Cluster.h"
#include "papas/datatypes/Particle.h"
#include "papas/datatypes/Path.h"
#include "papas/utility/TRandom.h"

namespace papas {

ClicHCAL::ClicHCAL(const VolumeCylinder& volume, const Material& material, double etacrack,
                 std::vector<std::vector<double>> eres, std::vector<std::vector<double>> eresp)
    : Calorimeter(Layer::kHcal, volume, material), m_etaCrack(etacrack), m_eres(eres), m_eresp(eresp) {}

ClicHCAL::ClicHCAL(const VolumeCylinder&& volume, const Material&& material, double etacrack,
                 std::vector<std::vector<double>> eres, std::vector<std::vector<double>> eresp)
    : Calorimeter(Layer::kHcal, volume, material), m_etaCrack(etacrack), m_eres(eres), m_eresp(eresp) {}

// will need to be rewritten for different detectors
/**
 Cluster_size as a function of the type of particle
 @param ptc particle
 @return size of resulting cluster
 */
double ClicHCAL::clusterSize(const Particle& ptc) const {
  (void)ptc;  // suppress warning messages for unused parameters;
  return 0.25;
}

/* Decides whether a cluster will be seen by a detector
 @param cluster the cluster to be analysed
 @return true is cluster is detected, false it if is too small to be seen
 */
bool ClicHCAL::acceptance(const Cluster& cluster) const {
  auto energy = cluster.energy();
  auto eta = fabs(cluster.position().Eta());
  if (eta < 2.76)  //TODO: check this value
                   return (energy>1.);
    else
      return false;}

double ClicHCAL::energyResolution(double energy, double eta) const {
  int part = kBarrel;
  double stoch = m_eres[part][0] / sqrt(energy);
  double noise = m_eres[part][1] / energy;
  double constant = m_eres[part][2];
  return sqrt(pow(stoch, 2) + pow(noise, 2) + pow(constant, 2));
}

double ClicHCAL::energyResponse(double energy, double eta) const {
  return 1;
}

}  // end namespace papas
