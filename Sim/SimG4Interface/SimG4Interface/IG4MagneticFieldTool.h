#ifndef Sim_IG4MagneticFieldTool_H
#define Sim_IG4MagneticFieldTool_H

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class G4MagneticField;

/** @class IG4MagneticFieldTool IG4MagneticFieldTool.h "SimG4Interface/IG4MagneticFieldTool.h"
 *  
 *  Abstract interface to Geant4 field classes
 * 
 *  @author Andrea Dell'Acqua
 *  @date   2016-02-22
 */

static const InterfaceID IID_IG4MagneticFieldTool( "IG4MagneticFieldTool" , 1 , 0 ) ;

class IG4MagneticFieldTool : virtual public IAlgTool {
 public:
  static const InterfaceID& interfaceID() { return IID_IG4MagneticFieldTool ; }

  /**  initialize
   *   @return status code
   */
  virtual StatusCode   initialize() = 0 ;

  /** get initilization hook for the geometry
   *  @return pointer to G4MagneticField
   */
  virtual G4MagneticField* getField() const = 0 ;
};
#endif
