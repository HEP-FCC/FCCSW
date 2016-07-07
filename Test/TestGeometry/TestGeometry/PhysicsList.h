#ifndef TESTGEOMETRY_PHYSICSLIST_H
#define TESTGEOMETRY_PHYSICSLIST_H

#include "TestGeometry/KillProcess.h"

// Geant
#include "G4VModularPhysicsList.hh"

/** @class PhysicsList TestGeometry/TestGeometry/PhysicsList.h PhysicsList.h
 *
 *  Physics list containing only transportation and a custom process, test::KillProcess
 *  that deposits all the energy in the electron origin (process added for electrons only).
 *
 *  @author Anna Zaborowska
 */

namespace test {
class PhysicsList: public G4VModularPhysicsList {
public:
  /// Constructor.
  PhysicsList();
  /// Destructor.
  virtual ~PhysicsList();
  /// Add the custom process that deposits all energy in the vertex.
  virtual void ConstructProcess() final;
  /// Construction of particles.
  virtual void ConstructParticle() final;
private:
  KillProcess m_process;
};
}

#endif /* TESTGEOMETRY_PHYSICSLIST_H */

