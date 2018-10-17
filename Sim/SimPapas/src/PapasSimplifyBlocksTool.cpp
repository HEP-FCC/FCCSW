#include "PapasSimplifyBlocksTool.h"
// papas
#include "papas/datatypes/Event.h"
#include "papas/reconstruction/SimplifyPFBlocks.h"

DECLARE_TOOL_FACTORY(PapasSimplifyBlocksTool)
/** @class PapasSimplifyBlocksTool Sim/SimPapasInterface/src/PapasSimplifyBlocksTool.h PapasSimplifyBlocksTool.h
 *
 *  Runs the papas block simplifying algorithm and adds the new simpified blocks into the papasevent.
 *  Block simplifying takes previous created blocks of tracks and clusters and tries to simplify the blocks
 *   (eg. by ensuring that at most one hcal is associated with a track) and then create smaller simpler
 *   blocks in some cases.
 *  @author A.J. Robson
 */
PapasSimplifyBlocksTool::PapasSimplifyBlocksTool(const std::string& aType, const std::string& aName,
                                                 const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IPapasTool>(this);
}

StatusCode PapasSimplifyBlocksTool::clear() {
  m_blocks.clear();
  return StatusCode::SUCCESS;
}

StatusCode PapasSimplifyBlocksTool::initialize() {
  debug() << "PAPAS block simplify options:" << m_blockSubtype << endmsg;
  return GaudiTool::initialize();
}

StatusCode PapasSimplifyBlocksTool::run(papas::Event& pevent, std::shared_ptr<papas::Detector> spDetector) {
  (void)spDetector;  // avoid warning
  auto history = papas::Nodes();
  std::string btype = m_blockSubtype;
  papas::simplifyPFBlocks(pevent, btype.c_str()[0], m_blocks, history);
  pevent.extendHistory(history);
  // add outputs into papasEvent
  pevent.addCollectionToFolder(m_blocks);
  return StatusCode::SUCCESS;
}

StatusCode PapasSimplifyBlocksTool::finalize() { return GaudiTool::finalize(); }
