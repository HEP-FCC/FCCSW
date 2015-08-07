// -------------------------------------------------------------------------
// File: Event/HepMCEvent.cpp
//
// AuthorList:
//
//         W. Pokorski: July 02 
// -------------------------------------------------------------------------
 
#include "Event/HepMCEvent.h"
//#include "HepMC/IO_Ascii.h"

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

// -------------------------------------------------------------------------
LHCb::HepMCEvent::HepMCEvent(const LHCb::HepMCEvent& evt):KeyedObject<int>() 
// -------------------------------------------------------------------------
{
  
  // deep copy
  *this = evt;
}

// -------------------------------------------------------------------------
LHCb::HepMCEvent& LHCb::HepMCEvent:: operator = (const LHCb::HepMCEvent& evt)
// -------------------------------------------------------------------------
{
  if(this != &evt) {
     m_generatorName = evt.generatorName();
     if(m_pGenEvt !=0) delete m_pGenEvt;
     const HepMC::GenEvent* ptr = evt.pGenEvt();
     m_pGenEvt = new HepMC::GenEvent(ptr->signal_process_id(),
              ptr->event_number());
  }
  return (*this);
}

