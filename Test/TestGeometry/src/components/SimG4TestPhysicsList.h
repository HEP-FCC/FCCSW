#ifndef TESTGEOMETRY_G4TESTPHYSICSLIST_H
#define TESTGEOMETRY_G4TESTPHYSICSLIST_H
// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimG4Interface/ISimG4PhysicsList.h"

/** @class SimG4TestPhysicsList TestGeometry/src/components/SimG4TestPhysicsList.h SimG4TestPhysicsList.h
 *
 *  Physics list tool. Creates test::TestPhysicsList.
 *
 *  @author Anna Zaborowska
 */

class SimG4TestPhysicsList : public AlgTool, virtual public ISimG4PhysicsList {
public:
  explicit SimG4TestPhysicsList(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimG4TestPhysicsList();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /** Get the physics list.
   *  @return pointer to G4VModularPhysicsList
   */
  virtual G4VModularPhysicsList* physicsList();
};

#endif /* TESTGEOMETRY_G4TESTPHYSICSLIST_H */
