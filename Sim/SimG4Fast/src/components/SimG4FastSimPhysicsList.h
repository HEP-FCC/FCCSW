#ifndef SIMG4FAST_G4FASTSIMPHYSICSLIST_H
#define SIMG4FAST_G4FASTSIMPHYSICSLIST_H

// Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimG4Interface/ISimG4PhysicsList.h"

/** @class SimG4FastSimPhysicsList SimG4Fast/src/components/SimG4FastSimPhysicsList.h SimG4FastSimPhysicsList.h
 *
 *  Fast simulation physics list tool.
 *  Attaches parametrisation process to the full simulation physics list.
 *
 *  @author Anna Zaborowska
 */

class SimG4FastSimPhysicsList : public AlgTool, virtual public ISimG4PhysicsList {
public:
  explicit SimG4FastSimPhysicsList(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimG4FastSimPhysicsList();

  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /** Get the physics list.
   *  @return pointer to G4VModularPhysicsList (ownership is transferred to the caller)
   */
  virtual G4VModularPhysicsList* physicsList();

private:
  /// Handle for the full physics list tool
  ToolHandle<ISimG4PhysicsList> m_physicsListTool{"SimG4FtfpBert", this, true};
};

#endif /* SIMG4FAST_G4FASTSIMPHYSICSLIST_H */
