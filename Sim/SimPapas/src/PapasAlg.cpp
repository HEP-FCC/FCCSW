#include "PapasAlg.h"

#include "SimPapas/IPapasTool.h"
#include "papas/utility/PDebug.h"
#include "papas/utility/TRandom.h"

#include <iostream>
DECLARE_COMPONENT(PapasAlg)

PapasAlg::PapasAlg(const std::string& name, ISvcLocator* svcLoc, int seed = 0) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("tools", m_toolNames);
  papas::PDebug::File("papaslog.log"); #todo make this able to use Gaudi logger
  m_eventno = 0;
  m_seed = seed;
}
StatusCode PapasAlg::initialize() {

  for (auto& toolname : m_toolNames) {
    m_tools.push_back(tool<IPapasTool>(toolname));
  }
  //allow the random generator to be seeded
  if (m_seed!=0)
    rootrandom::Random::seed(m_seed);
#if not WITHSORT
  debug() << "Papas: no sorting" << std::endl;
#endif
  return StatusCode::SUCCESS;
}

StatusCode PapasAlg::execute() {
  // setup new papas Event and history
  m_history.clear();
  papas::Event pevent(m_history);
  pevent.setEventNo(m_eventno);
  papas::PDebug::write("Event: {}", m_eventno);
  m_eventno++;

  // run the tools
  try {
    for (auto tool : m_tools) {
      tool->createOutputStructures();
    }
    for (auto tool : m_tools) {
      tool->run(pevent);
      debug() << "PAPAS " << tool->name() << std::endl << pevent.info() << std::endl;
    }
    for (auto tool : m_tools) {
      tool->clear();
    }
  } catch (std::string message) {
    warning() << "Event :" << m_eventno << " Exception thrown from gaudi papas tool" << message << std::endl;
    return StatusCode::SUCCESS;
  } catch (std::exception ex) {
    error() << "Exception thrown from gaudi papas tool" << std::endl;
    return StatusCode::FAILURE;  // see if it can continue
  }
  return StatusCode::SUCCESS;
}

StatusCode PapasAlg::finalize() { return GaudiAlgorithm::finalize(); }
