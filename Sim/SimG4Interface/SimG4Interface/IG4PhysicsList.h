#ifndef GIGA_IG4PHYSICSLIST_H
#define GIGA_IG4PHYSICSLIST_H 1 

/// Include files from the Framework
#include  "GaudiKernel/AlgTool.h"

// Forward declarations from Geant4
class G4VUserActionInitialization;


/** @class IG4ActionTool IG4ActionTool.h G4Interface/IG4ActionTool.h
 *
 *  abstract interface to load G4UserActionInitializations 
 *
 *  @author Benedikt HEGNER
 */

class IG4PhysicsList : virtual public AlgTool {
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() ;
  
  /**  initialize 
   *   @return status code 
   */
  virtual StatusCode   initialize() = 0 ;
  
  /**  initialize 
   *   @return status code 
   */
  virtual StatusCode   finalize() = 0 ;

  /** get initilization hook
   *  @return  pointer to G4VUserActionInitialization
   */
   virtual G4VUserActionInitialization* getUserActionInitialization() = 0 ;

protected:
  
  /// virtual destructor
  virtual ~IG4PhysicsList();

};

// ============================================================================
#endif 
// ============================================================================







































