#include "DataObjects/HepMCEntry.h"

HepMCEntry::~HepMCEntry() {
  if (m_event!=0) delete m_event;
}
