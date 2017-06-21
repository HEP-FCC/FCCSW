#ifndef FWCORE_PILEUPPARTICLESMERGETOOL_H
#define FWCORE_PILEUPPARTICLESMERGETOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "FWCore/IEDMMergeTool.h"

namespace fcc {
  class MCParticleCollection;
  class GenVertexCollection;
}

class PileupParticlesMergeTool: public GaudiTool, virtual public IEDMMergeTool {
public:
  explicit PileupParticlesMergeTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~PileupParticlesMergeTool() = default;

  virtual StatusCode initialize() override final;

  virtual StatusCode finalize() override final;

  virtual StatusCode readPileupCollection(podio::EventStore& store) override final;

  virtual StatusCode mergeCollections() override final;

private:

  Gaudi::Property<std::string> m_pileupGenParticlesBranchName{this, "genParticleBranch", "genParticles", ""}; 
  Gaudi::Property<std::string> m_pileupGenVerticesBranchName{this, "genVerticesBranch", "genVertices", ""}; 

  std::vector<const fcc::MCParticleCollection*> m_MCParticleCollections;
  std::vector<const fcc::GenVertexCollection*> m_GenVertexCollections;
};

#endif // FWCORE_PILEUPPARTICLESMERGETOOL_H
