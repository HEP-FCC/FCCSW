//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
#ifndef FCC_FAST_SIM_GEOMETRY_H
#define FCC_FAST_SIM_GEOMETRY_H

#include <vector>
#include "FCCDetectorConstruction.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4TransportationManager.hh"
#include "G4RegionStore.hh"
#include "G4GDMLParser.hh"
#include "G4SDManager.hh"
#include "G4UniformMagField.hh"

#include "FCCFastSimModelTracker.hh"
#include "FCCFastSimModelEMCal.hh"
#include "FCCFastSimModelHCal.hh"
#include "G4GlobalFastSimulationManager.hh"

/**
	@brief     Handling the auxiliary information from GDML file
   @details   Reads the auxiliary information from the GDML file. Creates the logical volumes based on the detector type and attaches fast simulation models. Creates the magnetic field.
   @author    Anna Zaborowska
*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class FCCFastSimGeometry
{
  public:
   /**
      A default constructor. Reads the map, creates the logical volumes for the detectors and attaches Fast Simulation Models (FCCFastSimModelTracker, FCCFastSimModelEMCal and FCCFastSimModelHCal). The magnetic field is created here.
      @param aAuxMap a map of auxiliary information (type and value) from GDML file.
    */
    FCCFastSimGeometry(const G4GDMLAuxMapType* aAuxMap);
    ~FCCFastSimGeometry();

private:
   /**
      A uniform magnetic field.
    */
    G4UniformMagField*        fField;
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
   // std::vector<FCCMuonSmearModel*> fMuonSmearModel;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
