// $Id: IFullGenEventCutTool.h,v 1.4 2005-12-31 17:30:37 robbep Exp $
#ifndef MCINTERFACES_IFULLGENEVENTCUTTOOL_H 
#define MCINTERFACES_IFULLGENEVENTCUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"

/** @class IFullGenEventCutTool IFullGenEventCutTool.h "MCInterfaces/IFullGenEventCutTool.h"
 *  
 *  Abstract interface to generator level cut on full event. This type of
 *  cut is applied on a fully generated event: it includes pile-up interactions
 *  and all particles have already been decayed.
 * 
 *  @author Patrick Robbe
 *  @date   2005-11-17
 */

static const InterfaceID IID_IFullGenEventCutTool( "IFullGenEventCutTool" , 
                                                   2 , 0 ) ;

class IFullGenEventCutTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IFullGenEventCutTool ; }

  /** Apply the cut on a event.
   *  @param[in] theEvents      Container of all interactions in the event.
   *  @param[in] theCollisions  Container of hard process informations of each 
   *                            pile-up interactions of the event.
   *  @return    true  if the full event passes the cut.
   */  
  virtual bool studyFullEvent( LHCb::HepMCEvents * theEvents ,
                               LHCb::GenCollisions * theCollisions ) 
    const = 0 ;
};
#endif // MCINTERFACES_ICUTTOOL_H
