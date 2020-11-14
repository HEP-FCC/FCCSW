#include "RiemannFitTool.h"

#include <utility>

#include "GaudiKernel/PhysicalConstants.h"

#include "edm4hep/TrackerHitCollection.h"
#include "edm4hep/TrackCollection.h"

#include "tricktrack/RiemannFit.h"
#include "tricktrack/TTPoint.h"

using Hit = tricktrack::TTPoint;

using namespace tricktrack;

DECLARE_COMPONENT(RiemannFitTool)

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

edm4hep::TrackCollection*
RiemannFitTool::fitTracks(const edm4hep::TrackerHits* theHits,
                          std::multimap<unsigned int, unsigned int> seedmap) {

  constexpr unsigned int nhits = 22;
  edm4hep::TrackCollection* tracks = new edm4hep::TrackCollection();

  decltype(seedmap.equal_range(0)) range;
  for (auto it1 = seedmap.begin(); it1 != seedmap.end(); it1 = range.second) {
    tricktrack::Matrix3xNd riemannHits = tricktrack::Matrix3xNd::Zero(3, nhits);
    auto track = tracks->create();
    unsigned int hitCounter = 0;
    // Get the range of the current TrackID
    range = seedmap.equal_range(it1->first);

    for (auto it2 = range.first; it2 != range.second; ++it2) {

      auto pos = (*theHits)[(*it2).second].position();
      if (hitCounter < nhits) {
        riemannHits.col(hitCounter) << pos.x, pos.y, pos.z;
      }
      hitCounter++;
    }
    if (m_doFit) {
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
        auto trackState = edm4hep::TrackState()
        trackState.setCovMatrix(fitcov);
        trackState.setPhi(h.par(0));
        trackState.setD0(h.par(1));
        trackState.setOmega(h.q / h.par(2));  // fit outputs pT
        trackState.setTanLambda(1. / h.par(3));         // fit outputs cotTheta
        trackState.setZ0(h.par(4));
        track.addToTrackStates(trackState);
    }
  }

  return tracks;
}

StatusCode RiemannFitTool::finalize() { return GaudiTool::finalize(); }
