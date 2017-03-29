#ifndef TESTGEOMETRY_PHYSICSLIST_H
#define TESTGEOMETRY_PHYSICSLIST_H

#include "TestGeometry/TestKillProcess.h"

// Geant
#include "G4VModularPhysicsList.hh"

/** @class TestPhysicsList TestGeometry/TestGeometry/TestPhysicsList.h TestPhysicsList.h
 *
 *  Physics list containing only transportation and a custom process, test::TestKillProcess
 *  that deposits all the energy in the electron origin (process added for electrons only).
 *
 *  @author Anna Zaborowska
 */

namespace test {
class TestPhysicsList : public G4VModularPhysicsList {
public:
  /// Constructor.
  TestPhysicsList();
  /// Destructor.
  virtual ~TestPhysicsList();
  /// Add the custom process that deposits all energy in the vertex.
  virtual void ConstructProcess() final;
  /// Construction of particles.
  virtual void ConstructParticle() final;

private:
  TestKillProcess m_process;
};
}

#endif /* TESTGEOMETRY_PHYSICSLIST_H */
