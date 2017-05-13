#ifndef SIMG4INTERFACE_ISIMG4ACTIONTOOL_H
#define SIMG4INTERFACE_ISIMG4ACTIONTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant4
class G4VUserActionInitialization;

/** @class ISimG4ActionTool SimG4Interface/SimG4Interface/ISimG4ActionTool.h ISimG4ActionTool.h
 *
 *  abstract interface to load initialization list of user actions
 *
 *  @author Benedikt HEGNER
 */

class ISimG4ActionTool : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  DeclareInterfaceID(ISimG4ActionTool, 1, 0);

  /** get initilization hook for the user action
   *  @return  pointer to G4VUserActionInitialization
   */
  virtual G4VUserActionInitialization* userActionInitialization() = 0;
};

#endif /* SIMG4INTERFACE_ISIMG4ACTIONTOOL_H */
