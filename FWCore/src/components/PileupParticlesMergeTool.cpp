#include "PileupParticlesMergeTool.h"

#include "podio/EventStore.h"

#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

DECLARE_TOOL_FACTORY(PileupParticlesMergeTool)

PileupParticlesMergeTool::PileupParticlesMergeTool(const std::string& aType, const std::string& aName,
                                                   const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IEDMMergeTool>(this);
  declareProperty("signalGenVertices", m_vertIn);
  declareProperty("signalGenParticles", m_partIn);
  declareProperty("mergedGenParticles", m_partOut);
  declareProperty("mergedGenVertices", m_vertOut);
}

StatusCode PileupParticlesMergeTool::initialize() { return StatusCode::SUCCESS; }

StatusCode PileupParticlesMergeTool::finalize() { return StatusCode::SUCCESS; }

StatusCode PileupParticlesMergeTool::readPileupCollection(podio::EventStore& store) {
  const fcc::MCParticleCollection* mcParticleCollection;
  const fcc::GenVertexCollection* genVertexCollection;

  bool mcParticleCollectionPresent = store.get(m_pileupGenParticlesBranchName, mcParticleCollection);
  debug() << "size of collection about to be read " << mcParticleCollection->size() << endmsg;
  if (mcParticleCollectionPresent) {
    m_MCParticleCollections.push_back(mcParticleCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupGenParticlesBranchName << endmsg;
  }

  bool genVertexCollectionPresent = store.get(m_pileupGenVerticesBranchName, genVertexCollection);
  if (genVertexCollectionPresent) {
    m_GenVertexCollections.push_back(genVertexCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupGenVerticesBranchName << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode PileupParticlesMergeTool::readSignal() {
  auto collVSig = m_vertIn.get();
  auto collPSig = m_partIn.get();

  m_MCParticleCollections.push_back(collPSig);
  m_GenVertexCollections.push_back(collVSig);

  return StatusCode::SUCCESS;
}

StatusCode PileupParticlesMergeTool::mergeCollections() {

  debug() << "merge " << m_GenVertexCollections.size() << "  collections ..." << endmsg;

  // ownership given to data service at end of execute
  fcc::MCParticleCollection* collPOut = new fcc::MCParticleCollection();
  fcc::GenVertexCollection* collVOut = new fcc::GenVertexCollection();

  for (auto genVertexColl : m_GenVertexCollections) {
    // copy vertices
    debug() << " traverse collection of size " << genVertexColl->size() << endmsg;
    for (const auto elem : *genVertexColl) {
      collVOut->push_back(elem.clone());
    }
  }
  for (auto mcPartColl : m_MCParticleCollections) {
    // copy particles
    for (const auto elem : *mcPartColl) {
      collPOut->push_back(elem.clone());
    }
  }

  m_vertOut.put(collVOut);
  m_partOut.put(collPOut);

  m_MCParticleCollections.clear();
  m_GenVertexCollections.clear();
  return StatusCode::SUCCESS;
}
