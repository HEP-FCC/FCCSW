#ifndef SIMG4FAST_FASTSIMPHYSICS_H
#define SIMG4FAST_FASTSIMPHYSICS_H

// Geant
#include "G4VPhysicsConstructor.hh"

/** @class FastSimPhysics SimG4Fast/SimG4Fast/FastSimPhysics.h FastSimPhysics.h
 *
 *  Physics constructor of the parametrisation process.
 *  It is meant to be registerd in the G4ModularPhysicsList.
 *
 *  @author Anna Zaborowska
 */

namespace sim {
class FastSimPhysics : public G4VPhysicsConstructor {
public:
  /// Constructor.
  FastSimPhysics();
  /** Constructor.
   *  @param aName Name of the physics constructor.
   */
  explicit FastSimPhysics(const G4String& aName);
  /// Destructor.
  virtual ~FastSimPhysics();
  /* Add the process of parametrisation to every existing particle
  * (created by the G4ModularPhysicsList to which it is registered)
  */
  virtual void ConstructProcess() final;
  /// Construction of particles. Nothing to be done by fast sim (parametrisation).
  virtual void ConstructParticle() final;
};
}

#endif /* SIMG4FAST_FASTSIMPHYSICS_H */
