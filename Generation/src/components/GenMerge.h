
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
  DataHandle<fcc::GenVertexCollection> m_vertInSignal{"signalGenParticles", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::MCParticleCollection> m_partInSignal{"signalGenVertices", Gaudi::DataHandle::Reader, this};

  DataHandle<fcc::GenVertexCollection> m_vertInPileUp{"pileupGenVertices", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::MCParticleCollection> m_partInPileUp{"pileupGenParticles", Gaudi::DataHandle::Reader, this};

  DataHandle<fcc::GenVertexCollection> m_vertOut{"allGenVertices", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::MCParticleCollection> m_partOut{"allGenParticles", Gaudi::DataHandle::Writer, this};
};

#endif  // GENERATION_GENMERGE_H
