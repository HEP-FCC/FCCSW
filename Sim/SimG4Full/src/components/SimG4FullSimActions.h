#ifndef SIMG4FULL_G4FULLSIMACTIONS_H
#define SIMG4FULL_G4FULLSIMACTIONS_H

// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimG4Interface/ISimG4ActionTool.h"

/** @class SimG4FullSimActions SimG4Full/src/components/SimG4FullSimActions.h SimG4FullSimActions.h
 *
 *  Tool for loading full simulation user action initialization (list of user actions)
 *
 *  @author Anna Zaborowska
 */

class SimG4FullSimActions : public AlgTool, virtual public ISimG4ActionTool {
public:
  explicit SimG4FullSimActions(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimG4FullSimActions();

  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /** Get the user action initialization.
   *  @return pointer to G4VUserActionInitialization (ownership is transferred to the caller)
   */
  virtual G4VUserActionInitialization* userActionInitialization() final;
};

#endif /* SIMG4FULL_G4FULLSIMACTIONS_H */
