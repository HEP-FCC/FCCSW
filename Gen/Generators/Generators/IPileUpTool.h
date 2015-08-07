// $Id: IPileUpTool.h,v 1.4 2009-04-07 16:18:32 gcorti Exp $
#ifndef GENERATORS_IPILEUPTOOL_H 
#define GENERATORS_IPILEUPTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class IPileUpTool IPileUpTool.h "Generators/IPileUpTool.h"
 *  
 *  Abstract interface to pile up tools. Generates the number of pile-up
 *  interactions to generate for each event.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_IPileUpTool( "IPileUpTool" , 3 , 0 ) ;

class IPileUpTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IPileUpTool ; }

  /** Computes the number of pile-up interactions in the event.
   *  @param[out] currentLuminosity  Luminosity of the current event.
   *  @return Number of pile-up interactions to generate.
   */
  virtual unsigned int numberOfPileUp( ) = 0 ;

  /// Print various counters at the end of the job
  virtual void printPileUpCounters( ) = 0 ;
};
#endif // GENERATORS_IPILEUPTOOL_H
