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

#ifndef FCC_PARTICLE_GUN_H
#define FCC_PARTICLE_GUN_H

#include "G4ParticleGunMessenger.hh"
#include "G4ParticleGun.hh"
#include "G4Event.hh"
#include "globals.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryVertex.hh"
#include "G4ParticleMomentum.hh"

/**
   @brief     Extension of G4ParticleGun to shoot particles in random directions.
   @details   An extension of G4ParticleGun. Shoots in a random direction a particle with given momentum and type).
 	@author    Anna Zaborowska
*/

class FCCParticleGun: public G4ParticleGun
{
public:
   /**
      A default constructor.
   */
   FCCParticleGun();
   virtual ~FCCParticleGun();
   /**
      Generates an event containing a particle just as in G4ParticleGun, additionally it sets a vertex position and momentum direction using randomly generated numbers. Attaches FCCPrimaryParticleInformation to all the primaries.
      @param aEvent A generated event.
   */
   void GeneratePrimaryVertex(G4Event* aEvent);
};

#endif

