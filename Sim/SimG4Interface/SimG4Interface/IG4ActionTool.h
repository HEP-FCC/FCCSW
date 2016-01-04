#ifndef SIM_IG4ACTIONTOOL_H
#define SIM_IG4ACTIONTOOL_H

// Gaudi
#include  "GaudiKernel/IAlgTool.h"

// Geant4
class G4VUserActionInitialization;


/** @class IG4ActionTool IG4ActionTool.h G4Interface/IG4ActionTool.h
 *
 *  abstract interface to load G4UserActionInitializations
 *
 *  @author Benedikt HEGNER
 */

class IG4ActionTool : virtual public IAlgTool {
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

#endif /* SIM_IG4ACTIONTOOL_H */
