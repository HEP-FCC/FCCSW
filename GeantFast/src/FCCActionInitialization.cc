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
// based on G4 examples/extended/parametrisations/Par01/src/Par01ActionInitialization.cc
//

#include "FCCActionInitialization.hh"
#include "FCCRunAction.hh"
#include "FCCEventAction.hh"
 #include "FCCTrackingAction.hh"
#include "G4UIcommand.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCActionInitialization::FCCActionInitialization() : G4VUserActionInitialization(), fFileName("DefaultOutput"), fSmear(true)
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCActionInitialization::FCCActionInitialization(const G4String aOutName, const G4String aSmear) : G4VUserActionInitialization(), fFileName(aOutName),fSmear(G4UIcommand::ConvertToBool(aSmear))
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCActionInitialization::FCCActionInitialization(const G4String aOutName) : G4VUserActionInitialization(), fFileName(aOutName),fSmear(true)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCActionInitialization::~FCCActionInitialization()
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCActionInitialization::BuildForMaster() const
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FCCActionInitialization::Build() const
{
   SetUserAction(new FCCRunAction(fFileName));
   SetUserAction(new FCCEventAction(fSmear));
   SetUserAction(new FCCTrackingAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
