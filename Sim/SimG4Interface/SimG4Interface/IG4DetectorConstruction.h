#ifndef SIM_IG4DETECTORCONSTRUCTION_H
#define SIM_IG4DETECTORCONSTRUCTION_H

/// Include files from the Framework
#include  "GaudiKernel/IAlgTool.h"

// Forward declarations from Geant4
class G4VUserDetectorConstruction;

/** @class IG4DetectorConstruction IG4DetectorConstruction.h SimG4Interface/DetectorConstruction.h
 *
 *  abstract interface to load G4VUserDetectorConstruction
 *
 *  @author Benedikt HEGNER
 */

class IG4DetectorConstruction : virtual public IAlgTool {
public:

  /// Retrieve interface ID
  DeclareInterfaceID(IG4DetectorConstruction,1,0) ;

  /**  initialize
   *   @return status code
   */
  virtual StatusCode   initialize() = 0 ;

  /** get initilization hook for the geometry
   *  @return pointer to G4VUserDetectorConstruction
   */
   virtual G4VUserDetectorConstruction* getDetectorConstruction() = 0 ;

};

#endif /*SIM_IG4DETECTORCONSTRUCTION_H */
