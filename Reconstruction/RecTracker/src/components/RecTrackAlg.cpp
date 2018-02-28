
#include "DetInterface/IGeoSvc.h"

#include "GaudiKernel/PhysicalConstants.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "DD4hep/Detector.h"
#include "DD4hep/Volumes.h"
#include "DDRec/API/IDDecoder.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include <cmath>
#include <random>

#include "RecInterface/ITrackSeedingTool.h"
#include "RecTrackAlg.h"
#include "RecTracker/TrackingUtils.h"

#include "tricktrack/RiemannFit.h"

DECLARE_ALGORITHM_FACTORY(RecTrackAlg)

RecTrackAlg::RecTrackAlg(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("positionedTrackHits", m_positionedTrackHits, "Tracker hits (Input)");
  declareProperty("Tracks", m_tracks, "Tracks (Output)");
  declareProperty("TrackStates", m_trackStates, "TrackStates (Output)");
  declareProperty("TrackSeedingTool", m_trackSeedingTool);
}

StatusCode RecTrackAlg::initialize() {
  debug() << "initialize" << endmsg;
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
  constexpr unsigned int nhits = 32;



	// Iterate through multimap of <tracks-hits> elements
	// range.first : first multimap element containing a range of hits with the same trackID
	// range.second : last multimap element of the range
  decltype(seedmap.equal_range(0)) range;
	for(auto it1 = seedmap.begin(); it1 != seedmap.end(); it1 = range.second){
    tricktrack::Matrix3xNd riemannHits = tricktrack::Matrix3xNd::Zero(3, nhits);
    auto track = tracks->create();
    track.bits((*it1).first);
    auto trackState = trackStates->create();
    unsigned int hitCounter = 0;
		// Get the range of the current TrackID
    range = seedmap.equal_range(it1->first);

		// Go through the range
		for(auto it2 = range.first; it2 != range.second; ++it2){
      track.addhits((*hits)[(*it2).second]);
      auto pos = (*hits)[(*it2).second].position();
      if (hitCounter < nhits) {
        riemannHits.col(hitCounter) << pos.x, pos.y, pos.z;
      }
      hitCounter++;
    }
    if (m_doFit) {
      if ((!m_fitOnlyPrimary) || (*it1).first == 1) {
        auto hitDim = std::min(hitCounter, nhits);
        auto resizedHits = riemannHits.block(0, 0, 3, hitDim);
        tricktrack::Matrix3Nd hits_cov = m_hitRes * tricktrack::Matrix3Nd::Identity(3 * hitDim, 3 * hitDim);

        constexpr double l_cSpeed = Gaudi::Units::c_light / Gaudi::Units::m * Gaudi::Units::s;
        const double l_bFfieldGeVCmC = m_Bz * l_cSpeed / pow(10, 9) / 100;  // conversion to GeV / cm / c
        auto h = tricktrack::Helix_fit(resizedHits, hits_cov, l_bFfieldGeVCmC, m_calcErrors, m_calcMultipleScattering);
        debug() << "Fit parameters: " << h.par(0) << "\t" << h.par(1) << "\t" << h.par(2) << "\t" << h.par(3) << "\t"
                << h.par(4) << endmsg;
        debug() << "Fit charge: " << h.q << "\t"
                << " chi_2 circle: " << h.chi2_circle << "\t"
                << "chi_2 line:  " << h.chi2_line << endmsg;
        debug() << " Fit covariance: " << endmsg;
        auto upperCov = h.cov.triangularView<Eigen::Upper>();
        debug() << Eigen::MatrixXd(upperCov) << endmsg;
        unsigned int coeffCounter = 0;
        /// write only unique values of 5 x 5 track parameter covariance to edm
        /// by convention upper triangle of matrix, flattened as below
        std::array<float, 15> fitcov;
        for (unsigned int i = 0; i < 5; ++i) {
          for (unsigned int j = i; j < 5; ++j) {
            fitcov[coeffCounter] = h.cov(j, i);
            coeffCounter++;
          }
        }
        trackState.cov(fitcov);
        trackState.phi(h.par(0));
        trackState.d0(h.par(1));
        trackState.qOverP(h.q / h.par(2));  // fit outputs pT
        trackState.theta(h.par(3));         // fit outputs cotTheta
        trackState.z0(h.par(4));
        track.addstates(trackState);
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode RecTrackAlg::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
