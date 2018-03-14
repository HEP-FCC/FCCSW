#ifndef SIMG4FAST_G4USERLIMITPHYSICSLIST_H
#define SIMG4FAST_G4USERLIMITPHYSICSLIST_H

// Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimG4Interface/ISimG4PhysicsList.h"

/** @class SimG4UserLimitPhysicsList SimG4Fast/src/components/SimG4UserLimitPhysicsList.h SimG4UserLimitPhysicsList.h
 *
 *  User limits physics list tool.
 *  Attaches G4StepLimiterPhysics process to the full simulation physics list.
 *
 *  @author Anna Zaborowska
 */

class SimG4UserLimitPhysicsList : public AlgTool, virtual public ISimG4PhysicsList {
public:
  explicit SimG4UserLimitPhysicsList(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimG4UserLimitPhysicsList();

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

#endif /* SIMG4FAST_G4USERLIMITPHYSICSLIST_H */
