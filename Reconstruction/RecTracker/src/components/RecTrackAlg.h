#ifndef RECTRACKER_RECTRACKALG_H
#define RECTRACKER_RECTRACKALG_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"

#include "FWCore/DataHandle.h"

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
  // INPUT: Trackhits
  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"TrackerPositionedHits", Gaudi::DataHandle::Reader,

                                                                      this};
  // OUTPUT: Tracks, TrackStates, and corresponding RecParticles 
  DataHandle<fcc::TrackStateCollection> m_trackStates{"TrackStates", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::TrackCollection> m_tracks{"Tracks", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::ParticleCollection> m_recParticles{"TrackRecoParticles", Gaudi::DataHandle::Writer, this};

  // TOOLS: Seeding- and Fit-Tools that do the actual work 
  ToolHandle<ITrackSeedingTool> m_trackSeedingTool{"TrickTrackSeedingTool/TrickTrackSeedingTool", this};
  ToolHandle<ITrackFittingTool> m_trackFittingTool{"RiemannFitTool/RiemannFitTool", this};
};

#endif /* RECTRACKER_RECTRACKALG_H */
