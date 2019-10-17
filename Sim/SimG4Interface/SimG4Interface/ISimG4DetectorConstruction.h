#ifndef SIMG4INTERFACE_ISIMG4DETECTORCONSTRUCTION_H
#define SIMG4INTERFACE_ISIMG4DETECTORCONSTRUCTION_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant4
class G4VUserDetectorConstruction;

/** @class ISimG4DetectorConstruction SimG4Interface/SimG4Interface/DetectorConstruction.h ISimG4DetectorConstruction.h
 *
 *  abstract interface to load G4VUserDetectorConstruction
 *
 *  @author Benedikt HEGNER
 */

class ISimG4DetectorConstruction : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  DeclareInterfaceID(ISimG4DetectorConstruction, 1, 0);

  /** get initilization hook for the geometry
   *  @return pointer to G4VUserDetectorConstruction
   */
  virtual G4VUserDetectorConstruction* detectorConstruction() = 0;
};

#endif /* SIMG4INTERFACE_ISIMG4DETECTORCONSTRUCTION_H */
