#ifndef DATAOBJECTS_GEANTEVENTENTRY_H
#define DATAOBJECTS_GEANTEVENTENTRY_H

//Include files
#include "GaudiKernel/DataObject.h"
#include "G4Event.hh"

class GAUDI_API GeantEventEntry : public DataObject {
 public:
  GeantEventEntry() : m_event(0){};
   virtual ~GeantEventEntry() {if (m_event!=0) delete m_event;};

  G4Event* getEvent() {return m_event;} 
  void setEvent(G4Event* event) {m_event = event;}

 private:
  G4Event* m_event; 


};


#endif
