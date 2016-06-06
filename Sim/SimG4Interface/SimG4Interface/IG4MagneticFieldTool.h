#ifndef SIMINTERFACE_IG4MAGNETICFIELDTOOL_H
#define SIMINTERFACE_IG4MAGNETICFIELDTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declaration:
// Geant 4:
class G4MagneticField;

/** @class IG4MagneticFieldTool IG4MagneticFieldTool.h "SimG4Interface/IG4MagneticFieldTool.h"
 *
 *  Abstract interface to Geant4 field classes
 *
 *  @author Andrea Dell'Acqua
 *  @date   2016-02-22
 */

// FIXME this tool should rather be a MagneticFieldSvc since it deals with G4 singletons

class IG4MagneticFieldTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IG4MagneticFieldTool, 1, 0);

  /** get initialization hook for the magnetic field
   *  @return pointer to G4MagneticField
   */
  virtual const G4MagneticField* field() const = 0;
};
#endif
