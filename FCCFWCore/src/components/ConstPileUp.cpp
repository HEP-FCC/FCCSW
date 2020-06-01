#include "ConstPileUp.h"

DECLARE_COMPONENT(ConstPileUp)

ConstPileUp::ConstPileUp(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IPileUpTool>(this);
}

ConstPileUp::~ConstPileUp() { ; }

StatusCode ConstPileUp::initialize() {
  StatusCode sc = GaudiTool::initialize();
  printPileUpCounters();
  return sc;
}

unsigned int ConstPileUp::numberOfPileUp() { return m_numPileUpEvents; }

double ConstPileUp::getMeanPileUp() { return m_numPileUpEvents; }

void ConstPileUp::printPileUpCounters() {
  info() << "Current number of pileup events: " << m_numPileUpEvents << endmsg;
}
