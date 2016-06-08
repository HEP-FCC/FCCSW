#ifndef SIMG4FAST_G4FASTSIMPHYSICSLIST_H
#define SIMG4FAST_G4FASTSIMPHYSICSLIST_H

// Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimG4Interface/IG4PhysicsList.h"

/** @class G4FastSimPhysicsList SimG4Fast/src/components/G4FastSimPhysicsList.h G4FastSimPhysicsList.h
 *
 *  Fast simulation physics list tool.
 *  Attaches parametrisation process to the full simulation physics list.
 *
 *  @author Anna Zaborowska
 */

class G4FastSimPhysicsList: public AlgTool, virtual public IG4PhysicsList {
public:
  explicit G4FastSimPhysicsList(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~G4FastSimPhysicsList();

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

private:
  /// Handle for the full physics list tool
  ToolHandle<IG4PhysicsList> m_physicsListTool;

};

#endif /* SIMG4FAST_G4FASTSIMPHYSICSLIST_H */
