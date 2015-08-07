// $Id: IBeamTool.h,v 1.4 2008-05-29 14:21:46 gcorti Exp $
#ifndef GENERATORS_IBEAMTOOL_H 
#define GENERATORS_IBEAMTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Vector3DTypes.h"

/** @class IBeamTool IBeamTool.h "Generators/IBeamTool.h"
 *  
 *  Abstract interface to beam tool providing beam parameters 
 *  for each generated event.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_IBeamTool( "IBeamTool" , 2 , 0 ) ;

class IBeamTool : virtual public IAlgTool {
 public:
  static const InterfaceID& interfaceID() { return IID_IBeamTool ; }
  
  /** Mean beam parameters.
   *  Provides mean beam parameters (useful to initialize generators).
   *  @param[out] pBeam1  Mean 3-momentum of beam 1 (pz > 0)
   *  @param[out] pBeam2  Mean 3-momentum of beam 2 (pz < 0)
   */
  virtual void getMeanBeams( Gaudi::XYZVector & pBeam1, 
                             Gaudi::XYZVector & pBeam2 ) const = 0 ;
  
  /** Generate beam parameters.
   *  @param[out] pBeam1  3-momentum of beam 1 (pz > 0)
   *  @param[out] pBeam2  3-momentum of beam 2 (pz > 0)
   */
  virtual void getBeams( Gaudi::XYZVector & pBeam1, 
                         Gaudi::XYZVector & pBeam2 ) = 0 ;
};
#endif // GENERATORS_IBEAMTOOL_H
