#include "PapasPFReconstructorTool.h"

#include "papas/detectors/CMS.h"
#include "papas/reconstruction/PFReconstructor.h"

DECLARE_TOOL_FACTORY(PapasPFReconstructorTool)

PapasPFReconstructorTool::PapasPFReconstructorTool(const std::string& aType, const std::string& aName,
                                                   const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IPapasTool>(this);
  declareProperty("blockSubtype", m_blockSubtype, "Blocks to use for reconstruction");
}

StatusCode PapasPFReconstructorTool::initialize() {
  // read and process a single event
  debug() << "PAPAS PFRECONSTRUCTOR options " << m_blockSubtype << std::endl;
  return GaudiTool::initialize();
}

StatusCode PapasPFReconstructorTool::clear() {
  m_rec_particles.clear();
  return StatusCode::SUCCESS;
}

StatusCode PapasPFReconstructorTool::run(papas::Event& pevent) {
  papas::Detector detector = papas::CMS();
  papas::Nodes history;
  std::string btype = m_blockSubtype;
  papas::PFReconstructor(pevent, btype.c_str()[0], detector, m_rec_particles, history);
  pevent.extendHistory(history);
  // add outputs into papasEvent
  pevent.addCollectionToFolder(m_rec_particles);
  debug() << "PAPAS Reconstructor" << std::endl << pevent.info() << std::endl;
  return StatusCode::SUCCESS;
}

StatusCode PapasPFReconstructorTool::finalize() { return GaudiTool::finalize(); }
