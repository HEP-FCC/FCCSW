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

#ifndef FCC_RUN_ACTION_H
#define FCC_RUN_ACTION_H

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

/**
	@brief     Run action (before/after run processing).
   @details   Defines the action at the beginning and at the end of each run. No content of G4Run can be changed. The class needs to be set in G4RunManager::SetUserAction().
 	@author    Anna Zaborowska
 */

class FCCRunAction : public G4UserRunAction
{
  public:
   /**
      A default constructor.
      @param OutName The output root file name. It will store all the events within run.
    */
    FCCRunAction(const G4String OutName);
    virtual ~FCCRunAction();

   /**
     Defines the actions at the beginning of the run. It starts the analysis (create output root file) and create all the histograms defined in FCCOutput singleton class.
    */
   virtual void BeginOfRunAction(const G4Run*);
   /**
      Defines the actions at the end of the run. It ends the analysis (write and close output root file) via FCCOutput singleton class.
   */
   virtual void   EndOfRunAction(const G4Run*);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
