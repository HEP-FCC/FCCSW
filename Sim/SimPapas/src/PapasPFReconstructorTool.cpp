#include "PapasPFReconstructorTool.h"
// PAPAS
#include "papas/detectors/Detector.h"
#include "papas/reconstruction/PFReconstructor.h"

DECLARE_TOOL_FACTORY(PapasPFReconstructorTool)

PapasPFReconstructorTool::PapasPFReconstructorTool(const std::string& aType, const std::string& aName,
                                                   const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IPapasTool>(this);
}

StatusCode PapasPFReconstructorTool::initialize() {
  debug() << "PAPAS pfreconstructor options:" << m_blockSubtype << endmsg;
  return GaudiTool::initialize();
}

StatusCode PapasPFReconstructorTool::clear() {
  m_recParticles.clear();
  return StatusCode::SUCCESS;
}

StatusCode PapasPFReconstructorTool::run(papas::Event& pevent, std::shared_ptr<papas::Detector> spDetector) {
  papas::Nodes history;
  std::string btype = m_blockSubtype;
  papas::PFReconstructor(pevent, btype.c_str()[0], *spDetector.get(), m_recParticles, history);
  pevent.extendHistory(history);
  // add outputs into papasEvent
  pevent.addCollectionToFolder(m_recParticles);
  return StatusCode::SUCCESS;
}

StatusCode PapasPFReconstructorTool::finalize() { return GaudiTool::finalize(); }
