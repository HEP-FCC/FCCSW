
#include "GenAlg.h"

#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

DECLARE_COMPONENT(GenAlg)

GenAlg::GenAlg(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("PileUpTool", m_pileUpTool);

  declareProperty("VertexSmearingTool", m_vertexSmearingTool);

  declareProperty("HepMCMergeTool", m_HepMCMergeTool);

  declareProperty("SignalProvider", m_signalProvider);
  declareProperty("PileUpProvider", m_pileUpProvider);

  declareProperty("hepmc", m_hepmchandle, "HepMC event handle (output)");
}

StatusCode GenAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc.isSuccess()) return sc;
  return sc;
}

StatusCode GenAlg::execute() {
  auto theEvent = m_hepmchandle.createAndPut();
  const unsigned int numPileUp = m_pileUpTool->numberOfPileUp();
  std::vector<HepMC::GenEvent> eventVector;
  eventVector.reserve(numPileUp + 1);

  StatusCode sc = m_signalProvider->getNextEvent(*theEvent);
  if (StatusCode::SUCCESS != sc) {
    return sc;
  }
  m_vertexSmearingTool->smearVertex(*theEvent);
  for (unsigned int i_pileUp = 0; i_pileUp < numPileUp; ++i_pileUp) {
    auto puEvt = HepMC::GenEvent();
    sc = m_pileUpProvider->getNextEvent(puEvt);
    if (StatusCode::SUCCESS != sc) {
      return sc;
    }
    m_vertexSmearingTool->smearVertex(puEvt);
    eventVector.push_back(std::move(puEvt));
  }
  return m_HepMCMergeTool->merge(*theEvent, eventVector);
}

StatusCode GenAlg::finalize() { return GaudiAlgorithm::finalize(); }
