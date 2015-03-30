#ifndef FCC_DETECTOR_CONSTRUCTION_H
#define FCC_DETECTOR_CONSTRUCTION_H

#include "FCCFastSimModelTracker.hh"
#include "FCCFastSimModelEMCal.hh"
#include "FCCFastSimModelHCal.hh"

#include "G4LogicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

/**
  @brief     Construction of detector geometry.
   @details   A mandatory initialization class of the detector setup. Detector construction allows to use the geometry read from the GDML file. Based on G4 examples/persistency/gdml/G01/include/G01DetectorConstruction.hh.
   @author    Anna Zaborowska
*/

class FCCDetectorConstruction : public G4VUserDetectorConstruction
{
public:
   /**
      A default constructor
   */
   FCCDetectorConstruction();
   virtual ~FCCDetectorConstruction();
  /**
      A method invoked by G4RunManager::Initialize()
      @return A pointer to the world volume.
   */
   virtual G4VPhysicalVolume* Construct();
   // virtual void ConstructSDandField();
   /**
      A vector of fast simulation models for a tracking detector.
    */
   std::vector<FCCFastSimModelTracker*> fTrackerSmearModel;
   /**
      A vector of fast simulation models for an electromagnetic calorimeter.
    */
   std::vector<FCCFastSimModelEMCal*> fEMCalSmearModel;
   /**
      A vector of fast simulation models for a hadronic calorimeter.
    */
   std::vector<FCCFastSimModelHCal*> fHCalSmearModel;
};


#endif

