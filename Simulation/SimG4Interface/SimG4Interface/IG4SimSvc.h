#ifndef SIMG4INTERFACE_IG4SIMSVC_H
#define SIMG4INTERFACE_IG4SIMSVC_H

// Gaudi
#include "GaudiKernel/IService.h"

// Geant
class G4Event;

/** @class IG4SimSvc SimG4Interface/SimG4Interface/IG4SimSvc.h IG4SimSvc.h
 *
 *  Interface to the main Geant simulation service.
 *
 *  @author Anna Zaborowska
 */

class IG4SimSvc: virtual public IService {
public:
  DeclareInterfaceID(IG4SimSvc,1,0);
  /**  Simulate the event with Geant.
   *   @param[in] aEvent An event to be processed.
   *   @return status code
   */
  virtual StatusCode processEvent(G4Event& aEvent) = 0;
  /**  Retrieve the processed event.
   *   @param[out] aEvent The processed event.
   *   @return status code
   */
  virtual StatusCode retrieveEvent(G4Event*& aEvent) = 0;
  /**  Terminate the event simulation.
   *   @return status code
   */
  virtual StatusCode terminateEvent() = 0;
};
#endif /* SIMG4INTERFACE_IG4SIMSVC_H */
