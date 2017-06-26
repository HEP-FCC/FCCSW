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

class PileupTrackHitsMergeTool : public GaudiTool, virtual public IEDMMergeTool {
public:
  explicit PileupTrackHitsMergeTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~PileupTrackHitsMergeTool() = default;

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
  Gaudi::Property<std::string> m_pileupTrackHitsBranchName{this, "pileupTrackHitsBranch", "trackHits",
                                                           "Name of the branch from which to read pileup collection"};
  /// Name of the branch from which to read pileup collection
  Gaudi::Property<std::string> m_pileupPosTrackHitsBranchName{
      this, "pileupPositionedTrackHitsBranch", "positionedTrackHits",
      "Name of the branch from which to read pileup collection"};

  /// internal container to keep track of collections to be merged
  std::vector<const fcc::TrackHitCollection*> m_TrackHitCollections;
  /// internal container to keep track of collections to be merged
  std::vector<const fcc::PositionedTrackHitCollection*> m_PosTrackHitCollections;

  /// Output of this tool: merged collection
  DataHandle<fcc::TrackHitCollection> m_TrackHitsMerged{"overlay/trackHits", Gaudi::DataHandle::Writer, this};
  /// Output of this tool: merged collection
  DataHandle<fcc::PositionedTrackHitCollection> m_PosTrackHitsMerged{"overlay/positionedTrackHits",
                                                                     Gaudi::DataHandle::Writer, this};

  /// input to this tool: signal collection
  DataHandle<fcc::TrackHitCollection> m_TrackHitsSignal{"overlay/signalTrackHits", Gaudi::DataHandle::Reader, this};
  /// input to this tool: signal collection
  DataHandle<fcc::PositionedTrackHitCollection> m_PosTrackHitsSignal{"overlay/signalPositionedTrackHits",
                                                                     Gaudi::DataHandle::Reader, this};
};

#endif  // FWCORE_PILEUPTRACKERHITSMERGETOOL_H
