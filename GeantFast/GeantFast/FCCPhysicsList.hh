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

#ifndef FCC_PHYSICS_LIST_H
#define FCC_PHYSICS_LIST_H

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

/**
	@brief     Construction of a physics list.
 	@details   A mandatory initialization class of the physics list. For the purposes of fast simulation, only transportation, decays and parametrisation is used. Based on G4 examples/extended/parametrisations/Par01/include/Par01PhysicsList.hh.
 	@author    Anna Zaborowska
 */

class FCCPhysicsList: public G4VUserPhysicsList
{
public:
   /**
      A default constructor. Sets the default cut value.
    */
  FCCPhysicsList();
  virtual ~FCCPhysicsList();

protected:
   /**
      Constructs particles: bosons, leptons, mesons, baryons and ions.
    */
  virtual void ConstructParticle();
   /**
      Constructs physics processes: particle transportation, parametrisation (for the purpose of fast parametric simulation) and decays.
    */
  virtual void ConstructProcess();

   /**
      Sets cuts with the default value for all particle types.
    */
  virtual void SetCuts();

protected:
   /**
      Constructs bosons: pseudo-particles such as geantino, as well as gamma.
    */
  virtual void ConstructBosons();
   /**
      Constructs all leptons.
    */
  virtual void ConstructLeptons();
   /**
      Constructs all mesons.
    */
  virtual void ConstructMesons();
   /**
      Constructs all barions.
    */
  virtual void ConstructBaryons();
   /**
      Constructs light ions.
    */
  virtual void ConstructIons();

protected:
   /**
      Creates a G4FastSimulationManagerProcess object for all the particle types.
    */
  void AddParameterisation();

   /**
      Adds decay process.
    */
  virtual void ConstructGeneral();
   /**
      Constructs processes for gamma, electron, positron, mu- and mu+.
    */
  virtual void ConstructEM();
   /**
      Adds the particle transport. G4CoupledTransportation is used to allow the calculation of the expected position of the particle within a G4VFastSimulationModel.
    */
  virtual void AddTransportation();
};

#endif



