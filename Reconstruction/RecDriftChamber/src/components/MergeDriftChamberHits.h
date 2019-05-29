#ifndef RECDRIFTCHAMBER_MERGEDRIFTCHAMBERHITS_H
#define RECDRIFTCHAMBER_MERGEDRIFTCHAMBERHITS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"
#include "FWCore/DataHandle.h"

namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}

class MergeDriftChamberHits : public GaudiAlgorithm {
public:
  MergeDriftChamberHits(const std::string& name, ISvcLocator* svcLoc);
  ~MergeDriftChamberHits() = default;
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  // temporary data structure used to accumulate hits with same trackID and cellID
  std::unordered_map<uint32_t, std::unordered_map<uint64_t, std::vector<fcc::PositionedTrackHit> > > m_track_cell_hit;

  DataHandle<fcc::PositionedTrackHitCollection> m_positionedHitsHandle_in{"DrifChamberHitsPositions", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::PositionedTrackHitCollection> m_mergedHitsPositionsHandle_out{"DriftChamberHitsPositionsMerged", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::TrackHitCollection> m_mergedHitsHandle_out{"DriftChamberHitsMerged", Gaudi::DataHandle::Writer, this};
};

#endif /* RECDRIFTCHAMBER_MERGEDRIFTCHAMBERHITS_H */
