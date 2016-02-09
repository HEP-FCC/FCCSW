#ifndef SIMG4FULL_G4FULLSIMACTIONS_H
#define SIMG4FULL_G4FULLSIMACTIONS_H

// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimG4Interface/IG4ActionTool.h"

/** @class G4FullSimActions SimG4Full/src/components/G4FullSimActions.h G4FullSimActions.h
 *
 *  Tool for loading full simulation user action initialization (list of user actions)
 *
 *  @author Anna Zaborowska
 */

class G4FullSimActions: public AlgTool, virtual public IG4ActionTool {
public:
   explicit G4FullSimActions(const std::string& type , const std::string& name,
               const IInterface* parent);
   virtual ~G4FullSimActions();

  /**  Initialize.
   *   @return status code
   */
   virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /** Get the user action initialization.
   *  @return pointer to G4VUserActionInitialization
   */
   virtual G4VUserActionInitialization* getUserActionInitialization() final;
};

#endif /* SIMG4FULL_G4FULLSIMACTIONS_H */
