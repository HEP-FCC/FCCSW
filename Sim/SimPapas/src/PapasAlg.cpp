#include "PapasAlg.h"

#include "SimPapas/IPapasTool.h"
#include "papas/utility/PDebug.h"
#include "papas/utility/TRandom.h"

#include <iostream>
DECLARE_COMPONENT(PapasAlg)

PapasAlg::PapasAlg(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("tools", m_toolNames);
  m_eventno = 0;
}

StatusCode PapasAlg::initialize() {

  for (auto& toolname : m_toolNames) {
    m_tools.push_back(tool<IPapasTool>(toolname));
  }
  if (m_physicsDebugFile != "") {
    debug() << "Papas: physics output " << m_physicsDebugFile << endmsg;
    papas::PDebug::File(m_physicsDebugFile);
  }
  // allow the random generator to be seeded
  if (m_seed != 0) {
    debug() << "Papas: set seed " << m_seed << endmsg;
    rootrandom::Random::seed(m_seed);
  }
#if not WITHSORT
  debug() << "Papas: no sorting" << endmsg;
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
      // create outputs structure (eg for export tool) so that Gaudi does not fail later on
      // at the data output stage (eg in the case where Papas does not finish simulation)
      tool->createOutputStructures();
    }
    for (auto tool : m_tools) {
      // run each tool
      tool->run(pevent);
    }
    // summary details of what the event contains
    debug() << "PAPAS event contains " << std::endl << pevent.info() << endmsg;
    // remove all the data ready for the next event
    for (auto tool : m_tools) {
      tool->clear();
    }
  } catch (std::string message) {
    // in case of problems
    warning() << "Event :" << m_eventno << "Exception was thrown from gaudi papas tool" << message << endmsg;
    debug() << "PAPAS event contains " << std::endl << pevent.info() << endmsg;
    for (auto tool : m_tools) {
      tool->clear();
    }
    return StatusCode::SUCCESS;
  } catch (std::exception ex) {
    error() << "Exception thrown from gaudi papas tool" << endmsg;
    return StatusCode::FAILURE;  // see if it can continue
  }
  return StatusCode::SUCCESS;
}

StatusCode PapasAlg::finalize() { return GaudiAlgorithm::finalize(); }
