#ifndef Clic_H
#define Clic_H

#include "papas/detectors/Detector.h"

namespace papas {

/**
 * @file Clic.h
 * @brief Implementation of Clic detector
 */
/// Clic specific implementation of Detector
class Clic : public Detector {
public:

  Clic(double innerEcal = 2.15, double outerEcal= 2.4, double innerHcal = 2.4,double outerHcal= 5.3);
  double electronAcceptance(const Track& track) const ;
  double electronEnergyResolution(const Particle& ptc) const ;
  double muonAcceptance(const Track& track) const ;
  double muonResolution(const Particle& ptc) const  ;
private:
};

}  // end namespace papas
#endif
