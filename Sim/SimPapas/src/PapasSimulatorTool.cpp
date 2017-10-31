#include "PapasSimulatorTool.h"
// FCCSW
#include "datamodel/GenVertex.h"
#include "datamodel/LorentzVector.h"
#include "datamodel/MCParticleCollection.h"
// PAPAS
#include "papas/detectors/CMS.h"
#include "papas/graphtools/DefinitionsNodes.h"
#include "papas/simulation/Simulator.h"
#include "papas/utility/PDebug.h"

DECLARE_TOOL_FACTORY(PapasSimulatorTool)

PapasSimulatorTool::PapasSimulatorTool(const std::string& aType, const std::string& aName, const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IPapasTool>(this);
}

PapasSimulatorTool::~PapasSimulatorTool() {}

StatusCode PapasSimulatorTool::initialize() {
  debug() << "PAPAS simulator options:" << m_particleSubtype << endmsg;
  return GaudiTool::initialize();
}

StatusCode PapasSimulatorTool::clear() {
  m_ecalClusters.clear();
  m_hcalClusters.clear();
  m_tracks.clear();
  m_smearedEcalClusters.clear();
  m_smearedHcalClusters.clear();
  m_smearedTracks.clear();
  return StatusCode::SUCCESS;
}

StatusCode PapasSimulatorTool::finalize() { return GaudiTool::finalize(); }

StatusCode PapasSimulatorTool::run(papas::Event& pevent) {

  auto detector = papas::CMS();  // todo consider making this be passed as part of the papas tool interface
  auto history = papas::Nodes();

  // run the simulator which will add information to the particles and fill the other collections
  std::string ptype = m_particleSubtype;
  papas::Simulator(pevent, ptype.c_str()[0], detector, m_ecalClusters, m_hcalClusters, m_smearedEcalClusters,
                   m_smearedHcalClusters, m_tracks, m_smearedTracks, history);
  pevent.extendHistory(history);
  // take the outputs and add these to the outgoing papasevent
  pevent.addCollectionToFolder(m_ecalClusters);
  pevent.addCollectionToFolder(m_hcalClusters);
  pevent.addCollectionToFolder(m_smearedEcalClusters);
  pevent.addCollectionToFolder(m_smearedHcalClusters);
  pevent.addCollectionToFolder(m_tracks);
  pevent.addCollectionToFolder(m_smearedTracks);
  return StatusCode::SUCCESS;
}
