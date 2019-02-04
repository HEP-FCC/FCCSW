#ifndef RECTRACKER_RECTRACKALG_H
#define RECTRACKER_RECTRACKALG_H

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
class ParticleCollection;
}

/***
 * Main track reconstruction algorithm, turning hits to track in a seeding and fitting stage.
 *  Uses tools to allow different implementation to do the actual work.
 */
class RecTrackAlg : public GaudiAlgorithm {
public:
  RecTrackAlg(const std::string& name, ISvcLocator* svcLoc);

  ~RecTrackAlg() = default;

  StatusCode initialize() override final;

  StatusCode execute() override final;

  StatusCode finalize() override final;

private:
  /// TrackHits as input to the track seeding
  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"TrackerPositionedHits", Gaudi::DataHandle::Reader,
                                                                      this};
  /// Tracks as output of the algorithm
  DataHandle<fcc::TrackCollection> m_tracks{"Tracks", Gaudi::DataHandle::Writer, this};
  /// Reconstructed particles as output of the algorithm
  DataHandle<fcc::ParticleCollection> m_recParticles{"TrackRecoParticles", Gaudi::DataHandle::Writer, this};
  /// TrackStates as output of the algorithm
  DataHandle<fcc::TrackStateCollection> m_trackStates{"TrackStates", Gaudi::DataHandle::Writer, this};
  /// Handle to Track Seeding Tool that does the work
  ToolHandle<ITrackSeedingTool> m_trackSeedingTool{"CombinatorialSeedingTool/CombinatorialSeedingTool", this};
  /// Handle to Track Fitting Tool that does the work
  ToolHandle<ITrackFittingTool> m_trackFittingTool{"RiemannFitTool/RiemannFitTool", this};
};

#endif /* RECTRACKER_RECTRACKALG_H */
