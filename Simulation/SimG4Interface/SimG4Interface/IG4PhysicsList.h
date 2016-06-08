#ifndef SIMG4INTERFACE_IG4PHYSICSLIST_H
#define SIMG4INTERFACE_IG4PHYSICSLIST_H

// Gaudi
#include  "GaudiKernel/IAlgTool.h"

// Geant4
class G4VModularPhysicsList;

/** @class IG4PhysicsList SimG4Interface/SimG4Interface/IG4PhysicsList.h IG4PhysicsList.h
 *
 *  abstract interface to load physics lists
 *
 *  @author Benedikt HEGNER
 */

class IG4PhysicsList: virtual public IAlgTool {
public:

  /// Retrieve interface ID
  DeclareInterfaceID(IG4PhysicsList,1,0);

  /**  initialize
   *   @return status code
   */
  virtual StatusCode   initialize() = 0;

  /** get initilization hook for the physics list
   *  @return  pointer to G4VModularPhysicsList
   */
   virtual G4VModularPhysicsList* getPhysicsList() = 0;

};

#endif /* SIMG4INTERFACE_IG4PHYSICSLIST_H */
