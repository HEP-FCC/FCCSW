#ifndef FWCORE_PILEUPCALOHITSMERGETOOL_H
#define FWCORE_PILEUPCALOHITSMERGETOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "FWCore/IEDMMergeTool.h"

namespace fcc {
  class CaloHitCollection;
  class PositionedCaloHitCollection;
}

class PileupCaloHitsMergeTool: public GaudiTool, virtual public IEDMMergeTool {
public:
  explicit PileupCaloHitsMergeTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~PileupCaloHitsMergeTool() = default;

  virtual StatusCode initialize() override final;

  virtual StatusCode finalize() override final;

  /// fill the member containers of collection pointers in this class with a collection from the event store
  virtual StatusCode readPileupCollection(podio::EventStore& store) override final;

  /// merge the collections in the member container and put them on  the event store
  virtual StatusCode mergeCollections() override final;

  /// fill the member container of collection pointer with a collection from the event store
  virtual StatusCode readSignal() override final;

private:
  /// Name of the branch from which to read pileup collection 
  Gaudi::Property<std::string> m_pileupCaloHitsBranchName{this, "pileupCaloHitsBranch", "caloHits", "Name of the branch from which to read pileup collection"}; 
  /// Name of the branch from which to read pileup collection 
  Gaudi::Property<std::string> m_pileupPosCaloHitsBranchName{this, "pileupPositionedCaloHitsBranch", "positionedCaloHits", "Name of the branch from which to read pileup collection"};

  /// internal container to keep track of collections to be merged
  std::vector<const fcc::CaloHitCollection*> m_CaloHitCollections;
  /// internal container to keep track of collections to be merged
  std::vector<const fcc::PositionedCaloHitCollection*> m_PosCaloHitCollections;

  /// Output of this tool: merged collection
  DataHandle<fcc::CaloHitCollection> m_CaloHitsMerged{"overlay/caloHits", Gaudi::DataHandle::Writer, this};
  /// Output of this tool: merged collection
  DataHandle<fcc::PositionedCaloHitCollection> m_PosCaloHitsMerged{"overlay/positionedCaloHits", Gaudi::DataHandle::Writer, this};

  /// input to this tool: signal collection
  DataHandle<fcc::CaloHitCollection> m_CaloHitsSignal{"overlay/signalCaloHits", Gaudi::DataHandle::Reader, this};
  /// input to this tool: signal collection
  DataHandle<fcc::PositionedCaloHitCollection> m_PosCaloHitsSignal{"overlay/signalPositionedCaloHits", Gaudi::DataHandle::Reader, this};
};

#endif // FWCORE_PILEUPCALOHITSMERGETOOL_H
