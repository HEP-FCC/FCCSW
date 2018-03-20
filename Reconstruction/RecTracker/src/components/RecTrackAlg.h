#ifndef RECTRACKER_COMBINATORIALSEEDINGTEST_H
#define RECTRACKER_COMBINATORIALSEEDINGTEST_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"

class IGeoSvc;
class ITrackSeedingTool;

namespace fcc {
class TrackHitCollection;
class TrackCollection;
class TrackStateCollection;
class PositionedTrackHitCollection;
}

/*** @class RecTrackAlg
 *
 */
class RecTrackAlg : public GaudiAlgorithm {
public:
  RecTrackAlg(const std::string& name, ISvcLocator* svcLoc);

  ~RecTrackAlg() = default;

  StatusCode initialize() override final;

  StatusCode execute() override final;

  StatusCode finalize() override final;

private:
  /// TrackHits as Input to the track seeding
  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedTrackHits", Gaudi::DataHandle::Reader,
                                                                      this};
  DataHandle<fcc::TrackCollection> m_tracks{"tracks", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::TrackStateCollection> m_trackStates{"trackStates", Gaudi::DataHandle::Writer, this};
  /// Handle to Track Seeding Tool that does the work
  Gaudi::Property<double> m_Bz{this, "Bz", 4., "Field strength along Z"};
  Gaudi::Property<double> m_hitRes{this, "hitRes", 0.1, "Resolution of local hit coordinates"};
  Gaudi::Property<bool> m_doFit{this, "doFit", true, "flag to actually perform the fit"};
  Gaudi::Property<bool> m_calcErrors{this, "calcErrors", true, "flag to actually calculate errors"};
  Gaudi::Property<bool> m_fitOnlyPrimary{this, "fitOnlyPrimary", false, "flag to only fit the particle with trackID 1"};
  Gaudi::Property<bool> m_calcMultipleScattering{this, "MultipleScatteringErrors", false,
                                                 "flag to toggle estimation of multiple scattering errors"};
  ToolHandle<ITrackSeedingTool> m_trackSeedingTool{"CombinatorialSeedingTool/CombinatorialSeedingTool", this};
};

#endif /* RECTRACKER_COMBINATORIALSEEDINGTEST_H */
