#include "RiemannFitTool.h"

#include <utility>

#include "GaudiKernel/PhysicalConstants.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "tricktrack/RiemannFit.h"
#include "tricktrack/TTPoint.h"

using Hit = tricktrack::TTPoint;

using namespace tricktrack;

DECLARE_TOOL_FACTORY(RiemannFitTool)

RiemannFitTool::RiemannFitTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ITrackFittingTool>(this);
}

StatusCode RiemannFitTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  return sc;
}

std::pair<fcc::TrackCollection*, fcc::TrackStateCollection*>
RiemannFitTool::fitTracks(const fcc::PositionedTrackHitCollection* theHits,
                          std::multimap<unsigned int, unsigned int> seedmap) {

  constexpr unsigned int nhits = 22;
  fcc::TrackCollection* tracks = new fcc::TrackCollection();
  fcc::TrackStateCollection* trackStates = new fcc::TrackStateCollection();

  decltype(seedmap.equal_range(0)) range;
  for (auto it1 = seedmap.begin(); it1 != seedmap.end(); it1 = range.second) {
    tricktrack::Matrix3xNd riemannHits = tricktrack::Matrix3xNd::Zero(3, nhits);
    auto track = tracks->create();
    auto trackState = trackStates->create();
    unsigned int hitCounter = 0;
    // Get the range of the current TrackID
    range = seedmap.equal_range(it1->first);

    unsigned int l_trackId = (*theHits)[(*it1).second].core().bits;
    // Go through the range
    for (auto it2 = range.first; it2 != range.second; ++it2) {
       //track.addhits((*theHits)[(*it2).second]); // TODO: reenable once new edm version available

      std::cout <<"trackIds: " << l_trackId << "\t" << std::endl;
      if (l_trackId != (*theHits)[(*it2).second].core().bits) {
        l_trackId = 0;
      }
      auto pos = (*theHits)[(*it2).second].position();
      if (hitCounter < nhits) {
        riemannHits.col(hitCounter) << pos.x, pos.y, pos.z;
      }
      hitCounter++;
    }
    if (l_trackId > 0) {
      track.bits(l_trackId);
    } else {
      track.bits(0);
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
        auto upperCov = h.cov.triangularView<Eigen::Upper>();
        debug() << " Fit covariance: "
                << "\n"
                << Eigen::MatrixXd(upperCov) << endmsg;
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
        trackState.theta(std::atan(1. / h.par(3)));         // fit outputs cotTheta
        //trackState.theta(h.par(3));
        trackState.z0(h.par(4));
        track.addstates(trackState);
      }
    }
  }

  return std::make_pair<fcc::TrackCollection*, fcc::TrackStateCollection*>(std::move(tracks), std::move(trackStates));
}

StatusCode RiemannFitTool::finalize() { return GaudiTool::finalize(); }
