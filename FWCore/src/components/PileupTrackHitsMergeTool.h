#ifndef FWCORE_PILEUPTRACKERHITSMERGETOOL_H
#define FWCORE_PILEUPTRACKERHITSMERGETOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "FWCore/IEDMMergeTool.h"

namespace fcc {
  class TrackHitCollection;
  class PositionedTrackHitCollection;
}

class PileupTrackHitsMergeTool: public GaudiTool, virtual public IEDMMergeTool {
public:
  explicit PileupTrackHitsMergeTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~PileupTrackHitsMergeTool() = default;

  virtual StatusCode initialize() override final;

  virtual StatusCode finalize() override final;

  virtual StatusCode readPileupCollection(podio::EventStore& store) override final;

  virtual StatusCode mergeCollections() override final;

  virtual StatusCode readSignal() override final;

private:

  Gaudi::Property<std::string> m_pileupTrackHitsBranchName{this, "pileupTrackHitsBranch", "trackHits", ""}; 
  Gaudi::Property<std::string> m_pileupPosTrackHitsBranchName{this, "pileupPositionedTrackHitsBranch", "positionedTrackHits", ""};

  std::vector<const fcc::TrackHitCollection*> m_TrackHitCollections;
  std::vector<const fcc::PositionedTrackHitCollection*> m_PosTrackHitCollections;

  DataHandle<fcc::TrackHitCollection> m_TrackHitsOut{"overlay/trackHits", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::PositionedTrackHitCollection> m_PosTrackHitsOut{"overlay/positionedTrackHits", Gaudi::DataHandle::Writer, this};

  DataHandle<fcc::TrackHitCollection> m_TrackHitsIn{"overlay/signalTrackHits", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::PositionedTrackHitCollection> m_PosTrackHitsIn{"overlay/signalPositionedTrackHits", Gaudi::DataHandle::Reader, this};
};

#endif // FWCORE_PILEUPTRACKERHITSMERGETOOL_H
