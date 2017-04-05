#ifndef TESTGEOMETRY_KILLPROCESS_H
#define TESTGEOMETRY_KILLPROCESS_H

// Geant
#include "G4VProcess.hh"

/** @class TestKillProcess TestGeometry/TestGeometry/TestKillProcess.h TestKillProcess.h
 *
 *  Deposit all the energy and kill the particle at the origin.
 *
 *  @author Anna Zaborowska
 */

namespace test {
class TestKillProcess : public G4VProcess {
public:
  /// Constructor.
  TestKillProcess(const std::string& aName = "G4TestKillProcess", G4ProcessType aType = fUserDefined);
  /// Destructor.
  virtual ~TestKillProcess(){};
  /// Add the custom process that deposits all energy in the vertex.
  virtual G4VParticleChange* AtRestDoIt(const G4Track&, const G4Step&) final { return nullptr; };
  virtual G4double AtRestGetPhysicalInteractionLength(const G4Track&, G4ForceCondition*) final { return -1; };
  virtual G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&) final;
  virtual G4double PostStepGetPhysicalInteractionLength(const G4Track&, G4double, G4ForceCondition*) final {
    return 0;
  };
  virtual G4VParticleChange* AlongStepDoIt(const G4Track&, const G4Step&) final { return nullptr; };
  virtual G4double AlongStepGetPhysicalInteractionLength(const G4Track&, G4double, G4double, G4double&,
                                                         G4GPILSelection*) final {
    return -1;
  };
};
}

#endif /* TESTGEOMETRY_KILLPROCESS_H */
