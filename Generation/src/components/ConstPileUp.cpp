
#include "ConstPileUp.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(ConstPileUp)

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

std::string ConstPileUp::getFilename() { return m_filename; }

void ConstPileUp::printPileUpCounters() {
  info() << "Current number of pileup events: " << m_numPileUpEvents << endmsg;
}
