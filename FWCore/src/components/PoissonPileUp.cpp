#include "PoissonPileUp.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"

DECLARE_TOOL_FACTORY(PoissonPileUp)

PoissonPileUp::PoissonPileUp(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IPileUpTool>(this);
}

PoissonPileUp::~PoissonPileUp() { ; }

StatusCode PoissonPileUp::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  if (m_meanPileUpEvents < 0) return Error("Number of Pileup events cannot be negative!");
  sc = m_PoissonDist.initialize(randSvc, Rndm::Poisson(m_meanPileUpEvents));
  if (!sc.isSuccess()) return Error("Could not initialize Poisson random number generator");
  release(randSvc);
  m_currentNumPileUpEvents = m_PoissonDist();
  printPileUpCounters();
  return sc;
}

std::string PoissonPileUp::getFilename() { return m_filename; }

unsigned int PoissonPileUp::numberOfPileUp() {
  m_currentNumPileUpEvents = m_PoissonDist();
  return m_currentNumPileUpEvents;
}

double PoissonPileUp::getMeanPileUp() { return m_meanPileUpEvents; }

void PoissonPileUp::printPileUpCounters() {
  info() << "Current number of pileup events:  " << m_currentNumPileUpEvents << endmsg;
}
