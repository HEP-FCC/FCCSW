/**
 * @file Clic.cpp
 * @brief Implementation of the Clic detector
 */
#include "Clic.h"

#include "ClicEcal.h"
#include "ClicField.h"
#include "ClicHcal.h"
#include "ClicTracker.h"

//#include "papas/detectors/Clic.h"
#include "papas/utility/PDebug.h"

//#include "papas/detectors/ClicEcal.h"
//#include "papas/detectors/ClicField.h"
//#include "papas/detectors/ClicHcal.h"
//#include "papas/detectors/ClicTracker.h"
#include "papas/datatypes/Track.h"
#include "papas/datatypes/Particle.h"

#include <cmath>

namespace papas {

Clic::Clic(double innerEcal, double outerEcal, double innerHcal, double outerHcal, std::shared_ptr<const Field> field) : Detector() {
  // ECAL detector Element
  PDebug::write("Detector: ecal inner {}, outer {}, hcal inner {} , outer{}", innerEcal, outerEcal, innerHcal, outerHcal);
  
  double depth = 0.25;
  double innerRadius = innerEcal;
  double innerZ = 2.6;
  int nX0 = 23;  //CLIC CDR, page 70, value for CLIC_ILD
  int nLambdaI = 1;  //ibid
  

  m_ecal = std::make_shared<const ClicECAL>(
      VolumeCylinder(Layer::kEcal, innerRadius+depth, innerZ + depth,innerRadius, innerZ),
      Material("Clic_ECAL",depth / nX0, depth / nLambdaI),
      1.479,                        // eta_crack
      std::vector<double>{0.5, 0.5},  // emin barrel and endcap
      std::vector<std::vector<double>>{{0.165, 0.010, 0.015},
                                       {-2.08048e-01, 3.25097e-01, 7.34244e-03}},
      std::vector<std::vector<double>>{{1.00071, -9.04973, -2.48554},
                                       {9.95665e-01, -3.31774, -2.11123}});  // barrel and endcap

  // HCAL detector element
  m_hcal = std::make_shared<const ClicHCAL>(
      VolumeCylinder(Layer::kHcal, outerHcal, 5.3, innerHcal, 2.85),
      Material("Clic_HCAL", 0.018, 0.17),
      1.3,  // eta crack
      std::vector<std::vector<double>>{{0.6, 0,0.025}, {0,0,0}},
      std::vector<std::vector<double>>{{0,0,0}, {0,0,0}});
  // Tracker detector element
  m_tracker = std::make_shared<const ClicTracker>(VolumeCylinder(Layer::kTracker, 2.14, 2.6));

  // Field detector element
  m_field = field;
  setupElements();  // sets up a list of all detector elements (m_elements) (needed for propagation)
}
  double Clic::electronAcceptance(const Track& track) const {
  return track.p3().Mag() > 5 && fabs(track.p3().Eta()) < 2.5;
}
  
  double Clic::electronEnergyResolution(const Particle& ptc) const { return 0.1 / sqrt(ptc.e()); }
  
  double Clic::muonAcceptance(const Track& track) const {
    ///returns True if muon is seen.
    /// The CLIC CDR gives 99% for E > 7.5GeV and polar angle > 10 degrees
    ///
      return (track.p3().Mag() > 7.5 &&
              fabs(track.theta()) < 80. * M_PI / 180.);
  }
  
  double Clic::muonResolution(const Particle& ptc) const {
    //return m_tracker->resolution(ptc); //todo discuss coline
    return 0.02;
  }
  
  

}  // end namespace papas
