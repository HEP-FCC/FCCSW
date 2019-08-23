
#include "DetInterface/IGeoSvc.h"

#include "GaudiKernel/PhysicalConstants.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"
#include "datamodel/ParticleCollection.h"

#include "DD4hep/Detector.h"
#include "DD4hep/Volumes.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include <cmath>
#include <random>

#include "RecInterface/ITrackSeedingTool.h"
#include "RecInterface/ITrackFittingTool.h"
#include "RecTrackAlg.h"
#include "RecTracker/TrackingUtils.h"


fcc::Particle TrackState2Particle(fcc::Track aTrack) {
  fcc::Particle theParticle;
  theParticle.bits(aTrack.bits());
  if (aTrack.states_size() > 0) {
    auto aState = aTrack.states(0);
    theParticle.charge((aState.qOverP() > 0) - (aState.qOverP() < 0));
    auto& p4 = theParticle.p4();
    p4.px = 1. / std::abs(aState.qOverP()) * std::cos(aState.phi());
    p4.py = 1. / std::abs(aState.qOverP()) * std::sin(aState.phi());
    p4.pz = std::tan(aState.theta()) * 1 / std::abs(aState.qOverP());
  }


  return theParticle;
}


DECLARE_COMPONENT(RecTrackAlg)

RecTrackAlg::RecTrackAlg(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("TrackerPositionedHits", m_positionedTrackHits, "Tracker hits (Input)");
  declareProperty("Tracks", m_tracks, "Tracks (Output)");
  declareProperty("TrackStates", m_trackStates, "TrackStates (Output)");
  declareProperty("TrackRecoParticles", m_recParticles, "TrackRecoParticles (Output)");
  declareProperty("TrackSeedingTool", m_trackSeedingTool);
  declareProperty("TrackFittingTool", m_trackFittingTool);
}

StatusCode RecTrackAlg::initialize() {
  debug() << "initialize" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode RecTrackAlg::execute() {

  // get hits from event store
  const fcc::PositionedTrackHitCollection* hits = m_positionedTrackHits.get();
  debug() << "Reconstructing Track Hit collection of size: " << hits->size() << " ..." << endmsg;
  // delegate track seeding to tool
  auto seedmap = m_trackSeedingTool->findSeeds(hits);
  // delegate track fitting to tool
  auto tracksAndTrackstates = m_trackFittingTool->fitTracks(hits, seedmap);

  // output found tracks to edm
  auto m_recParticleColl = m_recParticles.createAndPut();
  for (auto track: (*tracksAndTrackstates.first)) {
    auto p = TrackState2Particle(track);
    auto pEdm = m_recParticleColl->create();
    //pEdm = TrackState2Particle(track);
    pEdm.bits(p.bits());
    pEdm.charge(p.charge());
    pEdm.p4(p.p4());
  }
  m_tracks.put(tracksAndTrackstates.first);
  m_trackStates.put(tracksAndTrackstates.second);

  return StatusCode::SUCCESS;
}

StatusCode RecTrackAlg::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
