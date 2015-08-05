//Include files                                                                                                     

#include "DataObjects/PseudoJetEntry.h"


 PseudoJetEntry::~PseudoJetEntry() {
    if (m_jets != 0)
      delete m_jets;
 }

