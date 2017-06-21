#include "PapasBuildBlocksTool.h"

#include "papas/reconstruction/BuildPFBlocks.h"

DECLARE_TOOL_FACTORY(PapasBuildBlocksTool)

PapasBuildBlocksTool::PapasBuildBlocksTool(const std::string& aType, const std::string& aName,
                                           const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IPapasTool>(this);
}

StatusCode PapasBuildBlocksTool::initialize() {
  debug() << "PAPAS block builder options:" << m_ecalSubtype << m_hcalSubtype << m_trackSubtype << endmsg;

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
  return StatusCode::SUCCESS;
}

StatusCode PapasBuildBlocksTool::finalize() { return GaudiTool::finalize(); }
