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

  virtual StatusCode readPileupCollection(podio::EventStore& store) override final;

  virtual StatusCode mergeCollections() override final;

  virtual StatusCode readSignal() override final;

private:

  Gaudi::Property<std::string> m_pileupCaloHitsBranchName{this, "pileupCaloHitsBranch", "caloHits", ""}; 
  Gaudi::Property<std::string> m_pileupPosCaloHitsBranchName{this, "pileupPositionedCaloHitsBranch", "positionedCaloHits", ""};

  std::vector<const fcc::CaloHitCollection*> m_CaloHitCollections;
  std::vector<const fcc::PositionedCaloHitCollection*> m_PosCaloHitCollections;

  DataHandle<fcc::CaloHitCollection> m_CaloHitsOut{"overlay/caloHits", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::PositionedCaloHitCollection> m_PosCaloHitsOut{"overlay/positionedCaloHits", Gaudi::DataHandle::Writer, this};

  DataHandle<fcc::CaloHitCollection> m_CaloHitsIn{"overlay/signalCaloHits", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::PositionedCaloHitCollection> m_PosCaloHitsIn{"overlay/signalPositionedCaloHits", Gaudi::DataHandle::Reader, this};
};

#endif // FWCORE_PILEUPCALOHITSMERGETOOL_H
