#ifndef GEANT_FAST_PHYSICS_H
#define GEANT_FAST_PHYSICS_H

// Geant
#include "G4VPhysicsConstructor.hh"

/**
   @brief     Physics constructor enabling the parametrisation.
   @details   Physics constructor of the parametrisation process. It is meant to be used together with the G4ModularPhysicsList.
   @author    Anna Zaborowska
*/

class FastSimPhysics: public G4VPhysicsConstructor {
public:
   /**
      A constructor.
      @param aName A name of the physics constructor.
    */
   explicit FastSimPhysics(const G4String& aName);
   FastSimPhysics();
   virtual ~FastSimPhysics();
   virtual void ConstructProcess() final;
   virtual void ConstructParticle() final;
};

#endif

