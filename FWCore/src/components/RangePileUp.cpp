#include "RangePileUp.h"

DECLARE_COMPONENT(RangePileUp)

RangePileUp::RangePileUp(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IPileUpTool>(this);
}

RangePileUp::~RangePileUp() { ; }

StatusCode RangePileUp::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  return sc;
}

unsigned int RangePileUp::numberOfPileUp() {
  m_currentNumPileUpEvents = m_pileupRange[m_rangeIndex];
  m_rangeIndex = (m_rangeIndex + 1)  % m_pileupRange.size();
  return m_currentNumPileUpEvents;
}

double RangePileUp::getMeanPileUp() { return m_currentNumPileUpEvents; }

void RangePileUp::printPileUpCounters() {
  info() << "Current number of pileup events:  " << m_currentNumPileUpEvents << endmsg;
}
