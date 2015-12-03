#ifndef SIMG4COMMON_RUNMANAGER_H
#define SIMG4COMMON_RUNMANAGER_H

// Geant4
#include "G4RunManager.hh"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace sim {
  class RunManager: public G4RunManager {
public:
   /// Constructor.
   RunManager();
   /// Destructor.
   ~RunManager();
   /// Initialize.
   StatusCode start();
   /// Process event.
   bool processEvent(G4Event*);
   /// Retrieve an event.
   bool retrieveEvent(const G4Event*&);
   /// Terminate event.
   bool terminateEvent();
   /// Finalize.
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
