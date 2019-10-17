#ifndef SIMG4COMMON_RUNMANAGER_H
#define SIMG4COMMON_RUNMANAGER_H

// Geant4
#include "G4RunManager.hh"

// Gaudi
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

/** @class RunManager SimG4Common/SimG4Common/RunManager.h RunManager.h
 *
 *  Implementation of the main class for the simulation in Geant4.
 *  It allows GAUDI to control of the event flow.
 *  It is mandatory to set the geometry and physics list.
 *
 *  @author Anna Zaborowska
 */

namespace sim {
class RunManager : public G4RunManager {
public:
  /// Constructor.
  RunManager();
  /// Destructor.
  ~RunManager();
  /** Initialization.
   *  It substitutes the G4RunManager::BeamOn() method (excluding the loop over the events).
   *  Checks on the state of the Geant4 (proper initialization of the geometry, physics list, kernel and also if no
   * other simulation has been already running).
   *  @warning This method should be called \b after calling SetUserInitialization(G4VUserDetectorConstruction*) and
   * SetUserInitialization(G4VUserPhysicsList*).
   *  @returns the status code
   */
  StatusCode start();
  /** Processing of the event.
   *  It substitutes the G4RunManager::ProcessOneEvent(int) method (excluding the generation part).
   *  It checks if the previous event has been fully processed (including a call to terminateEvent()) and begins its
   * simulation.
   *  @warning Each call to processEvent() should be followed by a call to terminateEvent().
   *  @param[in] aEvent a generated event to be processed in a simulation
   *  @returns the status code
   */
  StatusCode processEvent(G4Event& aEvent);
  /** Retrieves an event.
   *  It allows to retrieve the event containing the data that may be stored (eg. collections of hits in the sensitive
   * detectors).
   *  The lifetime of the pointer to G4Event ends when method terminateEvent() is called.
   *  @param[out] aEvent a processed event
   *  @returns the status code
   */
  StatusCode retrieveEvent(G4Event*& aEvent);
  /** Termination of the event processing.
   *  @returns the status code
   */
  StatusCode terminateEvent();
  /// Finalization.
  void finalize();

private:
  /// Flag indicating if the previous Event was terminated in Geant successfuly
  bool m_prevEventTerminated;
  /// Message Service
  ServiceHandle<IMessageSvc> m_msgSvc;
  /// Message Stream
  MsgStream m_log;
};
}

#endif /* SIMG4COMMON_RUNMANAGER_H */
