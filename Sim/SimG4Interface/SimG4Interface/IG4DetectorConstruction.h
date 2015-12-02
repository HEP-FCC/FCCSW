#ifndef SIM_IG4DETECTORCONSTRUCTION_H
#define SIM_IG4DETECTORCONSTRUCTION_H 1 

/// Include files from the Framework
#include  "GaudiKernel/AlgTool.h"

// Forward declarations from Geant4
class G4VUserDetectorConstruction;

/** @class IG4DetectorConstruction IG4DetectorConstruction.h SimG4Interface/DetectorConstruction.h
 *
 *  abstract interface to load G4VUserDetectorConstruction
 *
 *  @author Benedikt HEGNER
 */

class IG4DetectorConstruction : virtual public AlgTool {
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
   *  @return pointer to G4VUserDetectorConstruction
   */
   virtual G4VUserDetectorConstruction* getDetectorConstruction() = 0 ;

protected:
  
  /// virtual destructor
  virtual ~IG4DetectorConstruction();

};

// ============================================================================
#endif 
// ============================================================================

