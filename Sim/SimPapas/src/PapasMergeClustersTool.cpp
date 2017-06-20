#include "PapasMergeClustersTool.h"

#include "papas/graphtools/EventRuler.h"
#include "papas/reconstruction/MergeClusters.h"

DECLARE_TOOL_FACTORY(PapasMergeClustersTool)

PapasMergeClustersTool::PapasMergeClustersTool(const std::string& aType, const std::string& aName,
                                               const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IPapasTool>(this);
}

StatusCode PapasMergeClustersTool::initialize() {
  // read and process a single event
  debug() << "PAPAS merger options " << m_typeSubtype << std::endl;
  return GaudiTool::initialize();
}

StatusCode PapasMergeClustersTool::clear() {
  m_merged_clusters.clear();
  return StatusCode::SUCCESS;
}

StatusCode PapasMergeClustersTool::run(papas::Event& pevent) {
  auto history = papas::Nodes();
  auto ruler = papas::EventRuler(pevent);
  papas::mergeClusters(pevent, m_typeSubtype, ruler, m_merged_clusters, history);
  pevent.extendHistory(history);
  pevent.addCollectionToFolder(m_merged_clusters);  // add outputs into papasEvent
  debug() << "PAPAS merger: " << m_typeSubtype << std::endl << pevent.info() << << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode PapasMergeClustersTool::finalize() { return GaudiTool::finalize(); }
