#ifndef SIMG4INTERFACE_ISIMG4MAGNETICFIELDTOOL_H
#define SIMG4INTERFACE_ISiIM4MAGNETICFIELDTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant 4:
#include "G4MagneticField.hh"

/** @class ISimG4MagneticFieldTool SimG4Interface/SimG4Interface/ISimG4MagneticFieldTool.h ISimG4MagneticFieldTool.h
 *
 *  Abstract interface to Geant4 field classes
 *
 *  @author Andrea Dell'Acqua
 *  @date   2016-02-22
 */

// FIXME this tool should rather be a MagneticFieldSvc since it deals with G4 singletons

class ISimG4MagneticFieldTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ISimG4MagneticFieldTool, 1, 0);

  /** get initialization hook for the magnetic field
   *  @return pointer to G4MagneticField
   */
  virtual const G4MagneticField* field() const = 0;
};

#endif /* SIMG4INTERFACE_ISiIM4MAGNETICFIELDTOOL_H */
