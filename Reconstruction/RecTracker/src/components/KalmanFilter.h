#ifndef RECTRACKER_KALMANFILTER_H
#define RECTRACKER_KALMANFILTER_H

#include "ActsExtrapolationTool.h"
// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"

#include "RecInterface/ITrackExtrapolationTool.h"

namespace fcc {
class TrackStateCollection;
class PositionedTrackHitCollection;
}

class KalmanFilter : public GaudiAlgorithm {
public:
  KalmanFilter(const std::string& name, ISvcLocator* svcLoc);

  ~KalmanFilter() = default;

  StatusCode initialize() override final;

  StatusCode execute() override final;

  StatusCode finalize() override final;

private:
  /// extrapolation tool
  ToolHandle<ActsExtrapolationTool> m_extrapolationTool{"ExtrapolationTool", this};

  /// input for the kalman fit
  DataHandle<fcc::TrackStateCollection> m_fittedTracks{"FittedTracks", Gaudi::DataHandle::Writer, this};

  DataHandle<fcc::TrackStateCollection> m_trackSeeds{"TrackSeeds", Gaudi::DataHandle::Reader, this};

  DataHandle<fcc::PositionedTrackHitCollection> m_hits{"TrackerHits", Gaudi::DataHandle::Reader, this};
};

#endif /* RECTRACKER_KALMANFILTER_H */
