#include "PapasAlg.h"
// FCCSW
#include "SimPapas/IPapasExportTool.h"
#include "SimPapas/IPapasTool.h"
#include "SimPapasDetector/IPapasDetSvc.h"
#include "papas/detectors/Detector.h"
// PAPAS
#include "papas/utility/Logger.h"
#include "papas/utility/PDebug.h"
#include "papas/utility/TRandom.h"

#include <iostream>
DECLARE_COMPONENT(PapasAlg)

PapasAlg::PapasAlg(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("tools", m_toolNames);
  declareProperty("exportTool", m_exportToolName);
  m_eventno = 0;
}

StatusCode PapasAlg::initialize() {
  for (auto& toolname : m_toolNames) {
    m_tools.push_back(tool<IPapasTool>(toolname));
  }
  m_exportTool = tool<IPapasExportTool>(m_exportToolName);
  SmartIF<IPapasDetSvc> papasDetSvc;  // curiously does not work if joined with following line
  papasDetSvc = service(m_detServiceName);
  if (!papasDetSvc) {
    error() << "Unable to locate Papas Detector Service " << m_detServiceName << endmsg;
    return StatusCode::FAILURE;
  }
  m_spDetector = papasDetSvc->detector();

  if (m_physicsDebugFile != "") {
    debug() << "Papas: physics output " << m_physicsDebugFile << endmsg;
    papas::PDebug::File(m_physicsDebugFile);
  }
  // allow the random generator to be seeded
  // todo make this use a Gaudi seed.
  if (m_seed != 0) {
    debug() << "Papas: set seed " << m_seed << endmsg;
    rootrandom::Random::seed(m_seed);
  }
#if not WITHSORT
  debug() << "Papas: no sorting" << endmsg;
#endif

  // setup the papas logger (ATS fashion)
  papaslog::papaslogger =
      papaslog::getDefaultLogger("PapasAlg",
                                 papaslog::Logging::ERROR);  // nb second value is dummy & not used at present
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
    // needed so that PODIO can find structure even if run is abandoned
    // and there are no particles created
    m_exportTool->createOutputStructures();
    // now do the real work
    for (auto tool : m_tools) {
      // run each tool
      tool->run(pevent, m_spDetector);
    }
    m_exportTool->run(pevent);
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
