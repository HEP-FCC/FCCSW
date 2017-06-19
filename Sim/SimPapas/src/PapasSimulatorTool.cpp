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

StatusCode PapasSimulatorTool::initialize() { return GaudiTool::initialize(); }

StatusCode PapasSimulatorTool::clear() {
  m_ecals.clear();
  m_hcals.clear();
  m_tracks.clear();
  m_smeared_ecals.clear();
  m_smeared_hcals.clear();
  m_smeared_tracks.clear();
  return StatusCode::SUCCESS;
}

StatusCode PapasSimulatorTool::finalize() { return GaudiTool::finalize(); }

StatusCode PapasSimulatorTool::run(papas::Event& pevent) {

  auto detector = papas::CMS();  // todo consider making this be passed as part of the papas tool interface
  auto history = papas::Nodes();

  // run the simulator which will add information to the particles and fill the other collections
  std::string ptype = m_particleSubtype;
  papas::Simulator(pevent, ptype.c_str()[0], detector, m_ecals, m_hcals, m_smeared_ecals, m_smeared_hcals, m_tracks,
                   m_smeared_tracks, history);
  pevent.extendHistory(history);
  // take the outputs and add these to the outgoing papasevent
  pevent.addCollectionToFolder(m_ecals);
  pevent.addCollectionToFolder(m_hcals);
  pevent.addCollectionToFolder(m_smeared_ecals);
  pevent.addCollectionToFolder(m_smeared_hcals);
  pevent.addCollectionToFolder(m_tracks);
  pevent.addCollectionToFolder(m_smeared_tracks);
  debug() << "PAPAS Blocks Simulator" << std::endl << pevent.info() << std::endl;
  return StatusCode::SUCCESS;
}
