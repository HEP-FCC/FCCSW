#ifndef SIMG4INTERFACE_IG4ACTIONTOOL_H
#define SIMG4INTERFACE_IG4ACTIONTOOL_H

// Gaudi
#include  "GaudiKernel/IAlgTool.h"

// Geant4
class G4VUserActionInitialization;


/** @class IG4ActionTool SimG4Interface/SimG4Interface/IG4ActionTool.h IG4ActionTool.h
 *
 *  abstract interface to load initialization list of user actions
 *
 *  @author Benedikt HEGNER
 */

class IG4ActionTool: virtual public IAlgTool {
public:

  /// Retrieve interface ID
  DeclareInterfaceID(IG4ActionTool,1,0);

  /**  initialize
   *   @return status code
   */
  virtual StatusCode initialize() = 0;

  /** get initilization hook for the user action
   *  @return  pointer to G4VUserActionInitialization
   */
   virtual G4VUserActionInitialization* getUserActionInitialization() = 0 ;

};

#endif /* SIMG4INTERFACE_IG4ACTIONTOOL_H */
