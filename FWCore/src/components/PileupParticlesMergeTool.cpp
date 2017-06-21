#include "PileupParticlesMergeTool.h"

#include "podio/EventStore.h" 

#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"


DECLARE_TOOL_FACTORY(PileupParticlesMergeTool)

PileupParticlesMergeTool::PileupParticlesMergeTool(const std::string& aType, const std::string& aName, const IInterface* aParent):
  GaudiTool(aType, aName, aParent) {
    declareInterface<IEDMMergeTool>(this);
    };


StatusCode PileupParticlesMergeTool::initialize() { return StatusCode::SUCCESS;}

StatusCode PileupParticlesMergeTool::finalize() { return StatusCode::SUCCESS;}

StatusCode PileupParticlesMergeTool::readPileupCollection(podio::EventStore& store) {
  const fcc::MCParticleCollection* mcParticleCollection;
  const fcc::GenVertexCollection* genVertexCollection;

  bool mcParticleCollectionPresent = store.get(m_pileupGenParticlesBranchName, mcParticleCollection);
  if (mcParticleCollectionPresent) {
    m_MCParticleCollections.push_back(mcParticleCollection);
  }

  bool genVertexCollectionPresent = store.get(m_pileupGenVerticesBranchName, genVertexCollection);
  if (genVertexCollectionPresent) {
    m_GenVertexCollections.push_back(genVertexCollection);
  }

  
  return StatusCode::SUCCESS;
}


StatusCode PileupParticlesMergeTool::mergeCollections() {

  // ownership given to data service at end of execute
  fcc::MCParticleCollection* collPOut = new fcc::MCParticleCollection();
  fcc::GenVertexCollection* collVOut = new fcc::GenVertexCollection();

  for (auto genVertexColl: m_GenVertexCollections) {
    // copy vertices
    for (const auto elem : *genVertexColl) {
      collVOut->push_back(elem.clone());
    }

  }
  for (auto mcPartColl: m_MCParticleCollections) {
    // copy pileup particles
    for (const auto elem : *mcPartColl) {
      collPOut->push_back(elem.clone());
    }
  }

  return StatusCode::SUCCESS;
}


