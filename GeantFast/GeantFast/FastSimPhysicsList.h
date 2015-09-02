#ifndef PHYSICS_LIST_H
#define PHYSICS_LIST_H

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

/**
  @brief     Construction of a physics list.
   @details   A mandatory initialization class of the physics list. For the purposes of fast simulation, only transportation, decays and parametrisation is used. Based on G4 examples/extended/parametrisations/Par01/include/Par01PhysicsList.hh.
   @author    Anna Zaborowska
 */

class PhysicsList: public G4VUserPhysicsList
{
public:
   /**
      A default constructor. Sets the default cut value.
    */
  PhysicsList();
  virtual ~PhysicsList();

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


