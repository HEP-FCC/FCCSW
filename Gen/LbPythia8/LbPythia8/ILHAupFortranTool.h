#ifndef LBPYTHIA8_ILHAUPFORTRANTOOL_H 
#define LBPYTHIA8_ILHAUPFORTRANTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class LbLHAupFortran ;

static const InterfaceID IID_ILHAupFortranTool ( "ILHAupFortranTool", 1, 0 );

/** @class ILHAupFortranTool ILHAupFortranTool.h LbPythia8/ILHAupFortranTool.h
 *  
 *  Interface for tools used to interface with Pythia Fortran User Processes
 *
 *  @author Patrick Robbe
 *  @date   2012-11-30
 */
class ILHAupFortranTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ILHAupFortranTool; }

  virtual LbLHAupFortran * getLHAupPtr( ) = 0 ;
  
};
#endif // LBPYTHIA8_ILHAUPFORTRANTOOL_H
