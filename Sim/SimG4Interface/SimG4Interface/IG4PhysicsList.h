#ifndef GIGA_IG4PHYSICSLIST_H
#define GIGA_IG4PHYSICSLIST_H 1 

/// Include files from the Framework
#include  "GaudiKernel/AlgTool.h"

// Forward declarations from Geant4
class G4VModularPhysicsList;

/** @class IG4PhysicsList IG4PhysicsList.h SimG4Interface/IG4PhysicsList.h
 *
 *  abstract interface to load physics lists
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
   *  @return  pointer to G4VModularPhysicsList
   */
   virtual G4VModularPhysicsList* getPhysicsList() = 0 ;

protected:
  
  /// virtual destructor
  virtual ~IG4PhysicsList();

};

// ============================================================================
#endif 
// ============================================================================
