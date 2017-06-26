#include "PileupCaloHitsMergeTool.h"

#include "podio/EventStore.h" 

#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"


DECLARE_TOOL_FACTORY(PileupCaloHitsMergeTool)

PileupCaloHitsMergeTool::PileupCaloHitsMergeTool(const std::string& aType, const std::string& aName, const IInterface* aParent):
  GaudiTool(aType, aName, aParent) {
    declareInterface<IEDMMergeTool>(this);
    declareProperty("signalCaloHits", m_CaloHitsSignal);
    declareProperty("signalPositionedCaloHits", m_PosCaloHitsSignal);
    declareProperty("mergedCaloHits", m_CaloHitsMerged);
    declareProperty("mergedPositionedCaloHits", m_PosCaloHitsMerged);
  }


StatusCode PileupCaloHitsMergeTool::initialize() { return StatusCode::SUCCESS;}

StatusCode PileupCaloHitsMergeTool::finalize() { return StatusCode::SUCCESS;}

StatusCode PileupCaloHitsMergeTool::readPileupCollection(podio::EventStore& store) {
  // local pointers, to be filled by the event store
  const fcc::CaloHitCollection* caloHitCollection;
  const fcc::PositionedCaloHitCollection* posCaloHitCollection;

  // get collection address and store it in container
  bool caloHitCollectionPresent = store.get(m_pileupCaloHitsBranchName, caloHitCollection);
  if (caloHitCollectionPresent) {
    m_CaloHitCollections.push_back(caloHitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupCaloHitsBranchName << endmsg;
    return StatusCode::FAILURE;
  }
  /// as above, for the positioned collection
  bool posCaloHitCollectionPresent = store.get(m_pileupPosCaloHitsBranchName, posCaloHitCollection);
  if (posCaloHitCollectionPresent) {
    m_PosCaloHitCollections.push_back(posCaloHitCollection);
  } else {
    warning() << "No collection could be read from branch " << m_pileupPosCaloHitsBranchName << endmsg;
    return StatusCode::FAILURE;
  }

  
  return StatusCode::SUCCESS;
}

StatusCode PileupCaloHitsMergeTool::readSignal() {
  // get collection from event sture
  auto collCaloHitsSig = m_CaloHitsSignal.get();
  auto collPosCaloHitsSig = m_PosCaloHitsSignal.get();

  // store them in internal container
  m_CaloHitCollections.push_back(collCaloHitsSig);
  m_PosCaloHitCollections.push_back(collPosCaloHitsSig);

  return StatusCode::SUCCESS;

}


StatusCode PileupCaloHitsMergeTool::mergeCollections() {

  debug() << "merge collections ..." << endmsg;

  // ownership given to data service at end of execute
  fcc::CaloHitCollection* collCaloHitsMerged = new fcc::CaloHitCollection();
  fcc::PositionedCaloHitCollection* collPosCaloHitsMerged = new fcc::PositionedCaloHitCollection();

  for (auto caloHitColl: m_CaloHitCollections) {
    // copy calo hits
    for (const auto elem : *caloHitColl) {
      collCaloHitsMerged->push_back(elem.clone());
    }

  }
  for (auto posCaloHitColl: m_PosCaloHitCollections) {
    // copy positioned calo hits
    for (const auto elem : *posCaloHitColl) {
      collPosCaloHitsMerged->push_back(elem.clone());
    }
  }

  m_CaloHitsMerged.put(collCaloHitsMerged);
  m_PosCaloHitsMerged.put(collPosCaloHitsMerged);

  m_CaloHitCollections.clear();
  m_PosCaloHitCollections.clear();
  return StatusCode::SUCCESS;
}


