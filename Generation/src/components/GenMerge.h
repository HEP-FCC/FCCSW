
#ifndef GENERATION_GENMERGE_H
#define GENERATION_GENMERGE_H

#include "Generation/IHepMCMergeTool.h"
#include "Generation/IHepMCProviderTool.h"
#include "Generation/IPileUpTool.h"
#include "Generation/IVertexSmearingTool.h"

#include "FWCore/DataHandle.h"

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/DataHandle.h"
#include "GaudiKernel/ToolHandle.h"

namespace fcc {
class GenVertexCollection;
class MCParticleCollection;
}

class GenMerge : public GaudiAlgorithm {
  friend class AlgFactory<GenMerge>;

public:
  /// Constructor.
  GenMerge(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  DataHandle<fcc::GenVertexCollection> m_vert_InSignal{"signalGenParticles", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::MCParticleCollection> m_part_InSignal{"signalGenVertices", Gaudi::DataHandle::Reader, this};

  DataHandle<fcc::GenVertexCollection> m_vert_InPileUp{"pileupGenVertices", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::MCParticleCollection> m_part_InPileUp{"pileupGenParticles", Gaudi::DataHandle::Reader, this};

  DataHandle<fcc::GenVertexCollection> m_vert_Out{"allGenVertices", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::MCParticleCollection> m_part_Out{"allGenParticles", Gaudi::DataHandle::Writer, this};
};

#endif  // GENERATION_GENMERGE_H
