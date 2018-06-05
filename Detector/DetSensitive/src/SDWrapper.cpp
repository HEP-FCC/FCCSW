#include "DD4hep/Detector.h"
#include "DDG4/Factories.h"

#include "DetSensitive/AggregateCalorimeterSD.h"
#include "DetSensitive/BirksLawCalorimeterSD.h"
#include "DetSensitive/FullParticleAbsorptionSD.h"
#include "DetSensitive/GflashCalorimeterSD.h"
#include "DetSensitive/MergeTrackerSD.h"
#include "DetSensitive/SimpleCalorimeterSD.h"
#include "DetSensitive/SimpleTrackerSD.h"

namespace dd4hep {
namespace sim {

// All G4VSensitiveDetector are deleted by ~G4SDManager called by ~G4RunManagerKernel
// Factory method to create an instance of SimpleTrackerSD
static G4VSensitiveDetector* create_simple_tracker_sd(const std::string& aDetectorName, dd4hep::Detector& aLcdd) {
  std::string readoutName = aLcdd.sensitiveDetector(aDetectorName).readout().name();
  return new det::SimpleTrackerSD(
      aDetectorName, readoutName, aLcdd.sensitiveDetector(aDetectorName).readout().segmentation());
}
// Factory method to create an instance of MergeTrackerSD
static G4VSensitiveDetector* create_merge_tracker_sd(const std::string& aDetectorName, dd4hep::Detector& aLcdd) {
  std::string readoutName = aLcdd.sensitiveDetector(aDetectorName).readout().name();
  return new det::MergeTrackerSD(
      aDetectorName, readoutName, aLcdd.sensitiveDetector(aDetectorName).readout().segmentation());
}
// Factory method to create an instance of SimpleCalorimeterSD
static G4VSensitiveDetector* create_simple_calorimeter_sd(const std::string& aDetectorName, dd4hep::Detector& aLcdd) {
  std::string readoutName = aLcdd.sensitiveDetector(aDetectorName).readout().name();
  return new det::SimpleCalorimeterSD(
      aDetectorName, readoutName, aLcdd.sensitiveDetector(aDetectorName).readout().segmentation());
}
// Factory method to create an instance of SimpleCalorimeterSD with Birks law
static G4VSensitiveDetector* create_birks_law_calorimeter_sd(const std::string& aDetectorName,
                                                             dd4hep::Detector& aLcdd) {
  std::string readoutName = aLcdd.sensitiveDetector(aDetectorName).readout().name();
  return new det::BirksLawCalorimeterSD(
      aDetectorName, readoutName, aLcdd.sensitiveDetector(aDetectorName).readout().segmentation());
}
// Factory method to create an instance of AggregateCalorimeterSD
static G4VSensitiveDetector* create_aggregate_calorimeter_sd(const std::string& aDetectorName,
                                                             dd4hep::Detector& aLcdd) {
  std::string readoutName = aLcdd.sensitiveDetector(aDetectorName).readout().name();
  return new det::AggregateCalorimeterSD(
      aDetectorName, readoutName, aLcdd.sensitiveDetector(aDetectorName).readout().segmentation());
}
// Factory method to create an instance of GflashCalorimeterSD
static G4VSensitiveDetector* create_gflash_calorimeter_sd(const std::string& aDetectorName, dd4hep::Detector& aLcdd) {
  std::string readoutName = aLcdd.sensitiveDetector(aDetectorName).readout().name();
  return new det::GflashCalorimeterSD(
      aDetectorName, readoutName, aLcdd.sensitiveDetector(aDetectorName).readout().segmentation());
}
// Factory method to create an instance of FullParticleAbsorptionSD
static G4VSensitiveDetector* create_full_particle_absorbtion_sd(const std::string& aDetectorName,
                                                                dd4hep::Detector& aLcdd) {
  std::string readoutName = aLcdd.sensitiveDetector(aDetectorName).readout().name();
  return new det::FullParticleAbsorptionSD(
      aDetectorName, readoutName, aLcdd.sensitiveDetector(aDetectorName).readout().segmentation());
}
}
}
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(SimpleTrackerSD, dd4hep::sim::create_simple_tracker_sd)
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(MergeTrackerSD, dd4hep::sim::create_merge_tracker_sd)
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(SimpleCalorimeterSD, dd4hep::sim::create_simple_calorimeter_sd)
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(BirksLawCalorimeterSD, dd4hep::sim::create_birks_law_calorimeter_sd)
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(AggregateCalorimeterSD, dd4hep::sim::create_aggregate_calorimeter_sd)
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(GflashCalorimeterSD, dd4hep::sim::create_gflash_calorimeter_sd)
DECLARE_EXTERNAL_GEANT4SENSITIVEDETECTOR(FullParticleAbsorptionSD, dd4hep::sim::create_full_particle_absorbtion_sd)
