
#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"
#include "datamodel/TrackCollection.h"

#include "DD4hep/Detector.h"
#include "DD4hep/Volumes.h"
#include "DDRec/API/IDDecoder.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include <cmath>
#include <random>

#include "RecTrackAlg.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecTracker/TrackingUtils.h"

#include "tricktrack/RiemannFit.h"

DECLARE_ALGORITHM_FACTORY(RecTrackAlg)

RecTrackAlg::RecTrackAlg(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc) {

  declareProperty("positionedTrackHits", m_positionedTrackHits, "Tracker hits (Input)");
  declareProperty("Tracks", m_tracks, "Tracks (Output)");
  declareProperty("TrackStates", m_trackStates, "TrackStates (Output)");
  declareProperty("TrackSeedingTool", m_trackSeedingTool);
}

StatusCode RecTrackAlg::initialize() {
  info() << "initialize" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode RecTrackAlg::execute() {

  // get hits from event store
  const fcc::PositionedTrackHitCollection* hits = m_positionedTrackHits.get();
  fcc::TrackCollection* tracks = m_tracks.createAndPut();
  fcc::TrackStateCollection* trackStates = m_trackStates.createAndPut();
  debug() << "hit collection size: " << hits->size() << endmsg;
  auto seedmap = m_trackSeedingTool->findSeeds(hits);
  auto it1 = seedmap.begin();
  auto it2 = seedmap.begin();
  auto end = seedmap.end();
  constexpr unsigned int nhits = 3;

  for (it1 = seedmap.begin(), it2 = it1, end = seedmap.end(); it1 != end; it1 = it2) {
    tricktrack::Matrix3xNd riemannHits = tricktrack::Matrix3xNd::Zero(3,nhits);
    debug() << " found trackseed: " << (*it1).first << "\t" << (*it1).second << endmsg;
    auto track = tracks->create();
    track.bits((*it1).first);
    auto trackState = trackStates->create();
    unsigned int hitCounter = 0;
    for ( ; it2 != end && (*it2).first == (*it1).first; ++it2) {
      debug() << " \t found trackseed: " << (*it2).first << "\t" << (*it2).second << endmsg;
      auto pos = (*hits)[(*it2).second].position();
      riemannHits.col(hitCounter) << pos.x, pos.y, pos.z;
      track.addhits((*hits)[(*it2).second]);
      hitCounter++;
      }
    tricktrack::Matrix3Nd hits_cov = tricktrack::Matrix3Nd::Random(3*nhits,3*nhits);

    auto h = tricktrack::Helix_fit(riemannHits, hits_cov, m_Bz, m_calcErrors, m_calcMultipleScattering);
    debug() << "Fit parameters: " <<  h.par << "\t"
     << "charge " << h.q << "\t" 
     << "chi_2 circle " << h.chi2_circle << "\t"
     << "chi_2 line " << h.chi2_line << endmsg;
    trackState.phi(h.par(0));
    trackState.d0(h.par(1));
    trackState.qOverP(h.q / h.par(2)); // fit outputs pT
    trackState.theta(h.par(3)); // fit outputs cotTheta
    trackState.z0(h.par(4));
    track.addstates(trackState);
    }

  return StatusCode::SUCCESS;
}

StatusCode RecTrackAlg::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
