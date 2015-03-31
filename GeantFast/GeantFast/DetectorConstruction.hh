#ifndef DETECTOR_CONSTRUCTION_H
#define DETECTOR_CONSTRUCTION_H

#include "FastSimModelTracker.hh"
#include "FastSimModelEMCal.hh"
#include "FastSimModelHCal.hh"

#include "G4LogicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

/**
  @brief     Construction of detector geometry.
   @details   A mandatory initialization class of the detector setup. Detector construction allows to use the geometry read from the GDML file. Based on G4 examples/persistency/gdml/G01/include/G01DetectorConstruction.hh.
   @author    Anna Zaborowska
*/

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
   /**
      A default constructor
   */
   DetectorConstruction();
   virtual ~DetectorConstruction();
  /**
      A method invoked by G4RunManager::Initialize()
      @return A pointer to the world volume.
   */
   virtual G4VPhysicalVolume* Construct();
   // virtual void ConstructSDandField();
   /**
      A vector of fast simulation models for a tracking detector.
    */
   std::vector<FastSimModelTracker*> fTrackerSmearModel;
   /**
      A vector of fast simulation models for an electromagnetic calorimeter.
    */
   std::vector<FastSimModelEMCal*> fEMCalSmearModel;
   /**
      A vector of fast simulation models for a hadronic calorimeter.
    */
   std::vector<FastSimModelHCal*> fHCalSmearModel;
};

#endif

