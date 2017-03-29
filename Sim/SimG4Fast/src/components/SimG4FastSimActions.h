#ifndef SIMG4FAST_G4FASTSIMACTIONS_H
#define SIMG4FAST_G4FASTSIMACTIONS_H

// Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimG4Interface/ISimG4ActionTool.h"

/** @class SimG4FastSimActions SimG4Fast/src/components/SimG4FastSimActions.h SimG4FastSimActions.h
 *
 *  Tool for loading fast simulation user action initialization (list of user actions)
 *
 *  @author Anna Zaborowska
*/

class SimG4FastSimActions : public AlgTool, virtual public ISimG4ActionTool {
public:
  explicit SimG4FastSimActions(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimG4FastSimActions();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  /** Get the user action initialization.
   *  @return pointer to G4VUserActionInitialization (ownership is transferred to the caller)
   */
  virtual G4VUserActionInitialization* userActionInitialization() final;
};

#endif /* SIMG4FAST_G4FASTSIMACTIONS_H */
