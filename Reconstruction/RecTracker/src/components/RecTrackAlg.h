#ifndef RECTRACKER_RECTRACKALG_H
#define RECTRACKER_RECTRACKALG_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"

#include "FWCore/DataHandle.h"

class ITrackSeedingTool;
class ITrackFittingTool;

namespace edm4hep {
class TrackerHitCollection;
class TrackCollection;
class ReconstructedParticleCollection;
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
  DataHandle<edm4hep::TrackerHitCollection> m_positionedTrackHits{"TrackerHits", Gaudi::DataHandle::Reader,

                                                                      this};
  // OUTPUT: Tracks, TrackStates, and corresponding RecParticles 
  DataHandle<edm4hep::TrackCollection> m_tracks{"Tracks", Gaudi::DataHandle::Writer, this};
  DataHandle<edm4hep::ReconstructedParticleCollection> m_recParticles{"TrackRecoParticles", Gaudi::DataHandle::Writer, this};

  // TOOLS: Seeding- and Fit-Tools that do the actual work 
  ToolHandle<ITrackSeedingTool> m_trackSeedingTool{"TrickTrackSeedingTool/TrickTrackSeedingTool", this};
  ToolHandle<ITrackFittingTool> m_trackFittingTool{"RiemannFitTool/RiemannFitTool", this};
};

#endif /* RECTRACKER_RECTRACKALG_H */
