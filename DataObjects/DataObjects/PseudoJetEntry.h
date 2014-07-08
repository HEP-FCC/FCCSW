#ifndef GENERATION_PSEUDOJETENTRY_H
#define GENERATION_PSEUDOJETENTRY_H

//Include files                                                                                                     
#include "GaudiKernel/DataObject.h"
#include "fastjet/PseudoJet.hh"
#include <vector>

class GAUDI_API PseudoJetEntry: public DataObject {
 public:

 PseudoJetEntry() :
  m_jets(0) {};

  virtual ~PseudoJetEntry() {
    if (m_jets != 0)
      delete m_jets;
  }

  std::vector<fastjet::PseudoJet>* getJets() {
    return m_jets;
  }

  void setJets(std::vector<fastjet::PseudoJet>* jets) {
    m_jets = jets;
  }

  void setJets(const std::vector<fastjet::PseudoJet> & jets) {
    m_jets = new std::vector<fastjet::PseudoJet>(jets);
  }

 private:
  std::vector<fastjet::PseudoJet> * m_jets;

};

#endif
