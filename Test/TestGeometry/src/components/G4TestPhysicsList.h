#ifndef TESTGEOMETRY_G4TESTPHYSICSLIST_H
#define TESTGEOMETRY_G4TESTPHYSICSLIST_H
// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimG4Interface/IG4PhysicsList.h"

/** @class G4TestPhysicsList TestGeometry/src/components/G4TestPhysicsList.h G4TestPhysicsList.h
 *
 *  Physics list tool. Creates test::PhysicsList.
 *
 *  @author Anna Zaborowska
 */

class G4TestPhysicsList: public AlgTool, virtual public IG4PhysicsList {
public:
  explicit G4TestPhysicsList(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~G4TestPhysicsList();
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
  virtual G4VModularPhysicsList* getPhysicsList();

};

#endif /* TESTGEOMETRY_G4TESTPHYSICSLIST_H */
