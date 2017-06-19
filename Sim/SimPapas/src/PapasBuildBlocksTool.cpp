#include "PapasBuildBlocksTool.h"

#include "papas/reconstruction/BuildPFBlocks.h"

DECLARE_TOOL_FACTORY(PapasBuildBlocksTool)

PapasBuildBlocksTool::PapasBuildBlocksTool(const std::string& aType, const std::string& aName,
                                           const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IPapasTool>(this);
  declareProperty("ecalSubtype", m_ecalSubtype, "Subtype of ecal clusters for block building");
  declareProperty("hcalSubtype", m_hcalSubtype, "Subtype of ecal clusters for block building");
  declareProperty("trackSubtype", m_trackSubtype, "Subtype of tracks for block building");
}

StatusCode PapasBuildBlocksTool::initialize() {
  // read and process a single event
  debug() << "PAPAS Blockbuilder options " << m_ecalSubtype << " + " << m_hcalSubtype << " + " << m_trackSubtype
         << std::endl;
  return GaudiTool::initialize();
}

StatusCode PapasBuildBlocksTool::clear() {
  m_blocks.clear();
  return StatusCode::SUCCESS;
}

StatusCode PapasBuildBlocksTool::run(papas::Event& pevent) {
  auto history = papas::Nodes();
  std::string etype = m_ecalSubtype;
  std::string htype = m_hcalSubtype;
  std::string ttype = m_trackSubtype;
  papas::buildPFBlocks(pevent, etype.c_str()[0], htype.c_str()[0], ttype.c_str()[0], m_blocks, history);
  pevent.extendHistory(history);
  // add outputs into papasEvent
  pevent.addCollectionToFolder(m_blocks);
  debug() << "PAPAS Blocks " << std::endl << pevent.info() << std::endl;
  return StatusCode::SUCCESS;
}

StatusCode PapasBuildBlocksTool::finalize() { return GaudiTool::finalize(); }
