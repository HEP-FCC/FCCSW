#ifndef RECTRACKER_RECHELIXTRAJECTORY_H
#define RECTRACKER_RECHELIXTRAJECTORY_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"

namespace edm4hep {
class TrackerHitCollection;
class TrackCollection;
}

/** @class RecHelixtrajectory
 *  Returns a list of equidistant points along the ideal helix a trackstate
 *  represents, for debugging and visualizations.
 */
class RecHelixTrajectory : public GaudiAlgorithm {
public:
  RecHelixTrajectory(const std::string& name, ISvcLocator* svcLoc);

  ~RecHelixTrajectory() = default;

  StatusCode initialize() override final;

  StatusCode execute() override final;

  StatusCode finalize() override final;

private:
  /// Output: Points along all the helices of the input tracks
  DataHandle<edm4hep::TrackerHitCollection> m_recHelixPoints{"RecHelixPoints", Gaudi::DataHandle::Writer, this};
  /// Input: TrackStates for which to calculate helix
  DataHandle<edm4hep::TrackCollection> m_trackStates{"TrackStates", Gaudi::DataHandle::Reader, this};
  ///
  Gaudi::Property<double> m_stepSize{this, "stepSize", 0.0001 * Gaudi::Units::mm};
  /// hard path length limit for the generated helix,
  // if it is longer that maxPathLength, stop adding points
  Gaudi::Property<double> m_maxPathLength{this, "maxPathLength", 2000000. * Gaudi::Units::mm};
  /// hard geometric limit for the generated helix,
  // stop adding points when the distance to the beamline is more than maxR
  Gaudi::Property<double> m_maxR{this, "maxR", 1600. * Gaudi::Units::mm};
  /// hard geometric limit for the generated helix,
  // stop adding points when the longitudinal distance from the origin is greater than maxZ
  Gaudi::Property<double> m_maxZ{this, "maxZ", 15000. * Gaudi::Units::mm};
};

#endif /* RECTRACKER_RECHELIXTRAJECTORY_H */
