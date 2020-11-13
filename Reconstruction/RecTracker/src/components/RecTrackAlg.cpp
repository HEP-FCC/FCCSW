#include "GaudiKernel/PhysicalConstants.h"

#include "edm4hep/TrackerHitCollection.h"
#include "edm4hep/TrackCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"

#include "RecInterface/ITrackSeedingTool.h"
#include "RecInterface/ITrackFittingTool.h"

#include "RecTrackAlg.h"


edm4hep::ReconstructedParticle TrackState2Particle(edm4hep::Track aTrack) {
  edm4hep::ReconstructedParticle theParticle;
  // currently just use first track state
  // @todo: proper treatment of case with several track states
  if (aTrack.states_size() > 0) {
    auto aState = aTrack.states(0);
    theParticle.setCharge((aState.qOverP() > 0) - (aState.qOverP() < 0));
    auto& p4 = theParticle.p4();
    theParticle.setMomentum({ 1. / std::abs(aState.qOverP()) * std::cos(aState.phi()),
        std::abs(aState.qOverP()) * std::sin(aState.phi()),
        std::tan(aState.theta()) * 1 / std::abs(aState.qOverP()),
    });
  }
  return theParticle;
}


DECLARE_COMPONENT(RecTrackAlg)

RecTrackAlg::RecTrackAlg(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("TrackerHits", m_trackHits, "Tracker hits (Input)");
  declareProperty("Tracks", m_tracks, "Tracks (Output)");
  declareProperty("TrackRecoParticles", m_recParticles, "TrackRecoParticles (Output)");
  declareProperty("TrackSeedingTool", m_trackSeedingTool);
  declareProperty("TrackFittingTool", m_trackFittingTool);
}

StatusCode RecTrackAlg::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode RecTrackAlg::execute() {
  // get hits from event store
  const edm4hep::TrackerHits* hits = m_trackHits.get();
  debug() << "Reconstructing Track Hit collection of size: " << hits->size() << " ..." << endmsg;
  // delegate track seeding to tool
  auto seedmap = m_trackSeedingTool->findSeeds(hits);
  // delegate track fitting to tool
  auto tracks = m_trackFittingTool->fitTracks(hits, seedmap);
  // output found tracks to edm
  auto m_recParticleColl = m_recParticles.createAndPut();
  for (auto track: (*tracks)) {
    auto p = TrackState2Particle(track);
    m_recParticleColl->push_back(p);
  }
  m_tracks.put(tracks);
  return StatusCode::SUCCESS;
}

StatusCode RecTrackAlg::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
