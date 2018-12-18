#ifndef RECTRACKER_RECHELIXTRAJECTORY_H
#define RECTRACKER_RECHELIXTRAJECTORY_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"

class IGeoSvc;
class ITrackSeedingTool;
class ITrackFittingTool;

namespace fcc {
class TrackHitCollection;
class TrackCollection;
class TrackStateCollection;
class PositionedTrackHitCollection;
}

/** @class RecHelixtrajectory
 *  Calculate points on the helix from the track state.
 */
class RecHelixTrajectory : public GaudiAlgorithm {
public:
  RecHelixTrajectory(const std::string& name, ISvcLocator* svcLoc);

  ~RecHelixTrajectory() = default;

  StatusCode initialize() override final;

  StatusCode execute() override final;

  StatusCode finalize() override final;

private:
  /// TrackHits as Input to the track seeding
  DataHandle<fcc::PositionedTrackHitCollection> m_recHelixPoints{"RecHelixPoints", Gaudi::DataHandle::Writer,
                                                                      this};
  DataHandle<fcc::PositionedTrackHitCollection> m_recExtrapolationPoint{"RecExtrapolationPoint", Gaudi::DataHandle::Writer,
                                                                      this};
  DataHandle<fcc::TrackCollection> m_tracks{"Tracks", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::TrackStateCollection> m_trackStates{"TrackStates", Gaudi::DataHandle::Reader, this};
  Gaudi::Property<double> m_stepSize {this, "stepSize", 0.0001};
  Gaudi::Property<double> m_maxPathLength {this, "maxPathLength", 2000000.};
  Gaudi::Property<double> m_maxR {this, "maxR", 1600. };
  Gaudi::Property<double> m_maxZ {this, "maxZ", 15000. };
};

#endif /* RECTRACKER_RECHELIXTRAJECTORY_H */
