#ifndef DATAOBJECTS_HEPMCENTRY_H
#define DATAOBJECTS_HEPMCENTRY_H

//Include files
#include "GaudiKernel/DataObject.h"
#include "HepMC/IO_GenEvent.h"

class GAUDI_API HepMCEntry : public DataObject {
 public:
  HepMCEntry() : m_event(0){};
  virtual ~HepMCEntry(); 

  const HepMC::GenEvent* getEvent() {return m_event;} 
  void setEvent(HepMC::GenEvent* event) {m_event = event;}

 private:
  HepMC::GenEvent* m_event; 


};


#endif
