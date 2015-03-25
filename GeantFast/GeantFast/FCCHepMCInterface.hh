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
// based on G4 examples/eventgenerator/HepMC/HepMCEx01/include/HepMCG4Interface.hh
//

#ifndef FCC_HEPMC_INTERFACE_H
#define FCC_HEPMC_INTERFACE_H

#include "G4VPrimaryGenerator.hh"
#include "HepMC/GenEvent.h"

/**
   @brief     Convertion of HepMC::GenEvent to G4Event.
   @details   A base class for event generation using a standard HepMC format. Converts a HepMC::GenEvent to G4event.
 	@author    Anna Zaborowska
*/

class FCCHepMCInterface : public G4VPrimaryGenerator
{
protected:
   /**
      An event in a standard HepMC format.
   */
   HepMC::GenEvent* fHepMCEvent;
   /**
      Should reassure that primary vertices are located inside the world volume.
      @param aPosition A position of the vertex.
   */
   virtual G4bool CheckVertexInsideWorld(const G4ThreeVector& aPosition) const;
   /**
      Convertes HepMC::GenEvent to G4Event. Attaches FCCPrimaryParticleInformation to all the primaries.
      @param aHepMCEvent An event in HepMC format to read from.
      @param aG4Event An event to write to.
   */
   void HepMC2G4(const HepMC::GenEvent* aHepMCEvent, G4Event* aG4Event);
   /**
      Creates an empty event by default. Should be implemented in derived classes.
   */
   virtual HepMC::GenEvent* GenerateHepMCEvent();

public:
   /**
      A default constructor.
   */
   FCCHepMCInterface();
   virtual ~FCCHepMCInterface();
   /**
      Gets the HepMC::GenEvent.
   */
   HepMC::GenEvent* GetHepMCGenEvent() const;
   /**
      Gets HepMC::GenEvent from GenerateHepMCEvent() method and converts it to G4Event.
      @param aEvent an event to convert.
   */
   virtual void GeneratePrimaryVertex(G4Event* aEvent);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
inline HepMC::GenEvent* FCCHepMCInterface::GetHepMCGenEvent() const
{
   return fHepMCEvent;
}

#endif
