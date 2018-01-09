
#ifndef GENERATION_GENMERGE_H
#define GENERATION_GENMERGE_H

#include "Generation/IHepMCMergeTool.h"
#include "Generation/IHepMCProviderTool.h"
#include "FWCore/IPileUpTool.h"
#include "Generation/IVertexSmearingTool.h"

#include "FWCore/DataHandle.h"

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/DataHandle.h"
#include "GaudiKernel/ToolHandle.h"

namespace fcc {
class GenVertexCollection;
class MCParticleCollection;
}

/** @class GenMerge
 *  @brief Algorithm merging two sets of MCParticle/Vertex-collections into one
 *  The main usecase, inspiring the member names, is overlaying generated pileup on a signal event. 
 *  Note that collections cannot be modified once created, thus all data must be cloned into a new collections.
 *  Associations between particles and vertices are updated, so it is safe to drop the old collections.
 */
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
