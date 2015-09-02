#ifndef FAST_SIM_PHYSICS_H
#define FAST_SIM_PHYSICS_H

#include "G4VPhysicsConstructor.hh"

class FastSimPhysics: public G4VPhysicsConstructor
{
public:
   FastSimPhysics(const G4String& aName);
   FastSimPhysics();
   virtual ~FastSimPhysics();
   virtual void ConstructProcess();
   virtual void ConstructParticle();
};

#endif

