#include "PileupCaloHitsMergeTool.h"

#include "podio/EventStore.h" 

#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"


DECLARE_TOOL_FACTORY(PileupCaloHitsMergeTool)

PileupCaloHitsMergeTool::PileupCaloHitsMergeTool(const std::string& aType, const std::string& aName, const IInterface* aParent):
  GaudiTool(aType, aName, aParent) {
    declareInterface<IEDMMergeTool>(this);
    declareProperty("signalCaloHits", m_CaloHitsIn);
    declareProperty("signalPositionedCaloHits", m_PosCaloHitsIn);
    declareProperty("allCaloHits", m_CaloHitsOut);
    declareProperty("allPositionedCaloHits", m_PosCaloHitsOut);
  }


StatusCode PileupCaloHitsMergeTool::initialize() { return StatusCode::SUCCESS;}

StatusCode PileupCaloHitsMergeTool::finalize() { return StatusCode::SUCCESS;}

StatusCode PileupCaloHitsMergeTool::readPileupCollection(podio::EventStore& store) {
  const fcc::CaloHitCollection* caloHitCollection;
  const fcc::PositionedCaloHitCollection* posCaloHitCollection;

  bool caloHitCollectionPresent = store.get(m_pileupCaloHitsBranchName, caloHitCollection);
  if (caloHitCollectionPresent) {
    m_CaloHitCollections.push_back(caloHitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupCaloHitsBranchName << endmsg;
  }

  bool posCaloHitCollectionPresent = store.get(m_pileupPosCaloHitsBranchName, posCaloHitCollection);
  if (posCaloHitCollectionPresent) {
    m_PosCaloHitCollections.push_back(posCaloHitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupPosCaloHitsBranchName << endmsg;
  }

  
  return StatusCode::SUCCESS;
}

StatusCode PileupCaloHitsMergeTool::readSignal() {
  auto collCaloHitsSig = m_CaloHitsIn.get();
  auto collPosCaloHitsSig = m_PosCaloHitsIn.get();

  m_CaloHitCollections.push_back(collCaloHitsSig);
  m_PosCaloHitCollections.push_back(collPosCaloHitsSig);

  return StatusCode::SUCCESS;

}


StatusCode PileupCaloHitsMergeTool::mergeCollections() {

  debug() << "merge collections ..." << endmsg;

  // ownership given to data service at end of execute
  fcc::CaloHitCollection* collCaloHitsOut = new fcc::CaloHitCollection();
  fcc::PositionedCaloHitCollection* collPosCaloHitsOut = new fcc::PositionedCaloHitCollection();

  for (auto caloHitColl: m_CaloHitCollections) {
    // copy calo hits
    for (const auto elem : *caloHitColl) {
      collCaloHitsOut->push_back(elem.clone());
    }

  }
  for (auto posCaloHitColl: m_PosCaloHitCollections) {
    // copy positioned calo hits
    for (const auto elem : *posCaloHitColl) {
      collPosCaloHitsOut->push_back(elem.clone());
    }
  }

  m_CaloHitsOut.put(collCaloHitsOut);
  m_PosCaloHitsOut.put(collPosCaloHitsOut);

  m_CaloHitCollections.clear();
  m_PosCaloHitCollections.clear();
  return StatusCode::SUCCESS;
}


