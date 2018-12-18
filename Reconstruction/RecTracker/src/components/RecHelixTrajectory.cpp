
#include "DetInterface/IGeoSvc.h"

#include "GaudiKernel/PhysicalConstants.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"
#include "datamodel/Point.h"

#include <cmath>
#include <random>

#include "RecHelixTrajectory.h"
#include "RecTracker/TrackingUtils.h"



DECLARE_ALGORITHM_FACTORY(RecHelixTrajectory)

RecHelixTrajectory::RecHelixTrajectory(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("RecHelixPoints", m_recHelixPoints, "Tracker hits (Output)");
  declareProperty("RecExtrapolationPoint", m_recExtrapolationPoint, "Tracker hits (Output)");
  declareProperty("Tracks", m_tracks, "Tracks (Input)");
  declareProperty("TrackStates", m_trackStates, "TrackStates (Input)");
}

StatusCode RecHelixTrajectory::initialize() {
  debug() << "initialize" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode RecHelixTrajectory::execute() {

  // get trackstates from event store
  const fcc::TrackStateCollection* trackStates = m_trackStates.get();
  fcc::PositionedTrackHitCollection* points = m_recHelixPoints.createAndPut();
  fcc::PositionedTrackHitCollection* extrapolationPoint = m_recExtrapolationPoint.createAndPut();

  unsigned int maxSteps = m_maxPathLength / m_stepSize;
  for (auto trackState: (*trackStates)) {
    double q_pT = trackState.qOverP() * -10; // TODO
    double cotTheta = trackState.theta();
    double phi0 = trackState.phi();
    double d0 = trackState.d0();
    double z0 = trackState.z0();

    double charge = (q_pT > 0) - (q_pT < 0); // get charge from sign of q over pt
    double rho = charge / std::abs(q_pT) / 0.0003 / 4.;
    double xc = (rho - d0) * std::cos(phi0 + 0.5 * M_PI);
    double yc = (rho - d0) * std::sin(phi0 + 0.5 * M_PI);
    for (double  i = 0; i < maxSteps; i += m_stepSize) {
      fcc::Point l_helixPoint = fcc::Point();
      fcc::BareHit l_corehit = fcc::BareHit();
      
      l_helixPoint.x = xc + rho * std::cos(charge * i  + phi0 - M_PI * 0.5);
      l_helixPoint.y = yc + rho * std::sin(charge * i  + phi0 - M_PI * 0.5);
      l_helixPoint.z = z0 + cotTheta * std::abs(rho) * i ;
      debug() << l_helixPoint.x << "\t" << l_helixPoint.y << "\t" << l_helixPoint.z << endmsg; 
      points->create(l_helixPoint, l_corehit);
      if (std::abs(l_helixPoint.z) > m_maxZ) {
        extrapolationPoint->create(l_helixPoint, l_corehit);
        break;
      }
      if (std::sqrt(std::pow(l_helixPoint.x,2) + std::pow(l_helixPoint.y,2)) > m_maxR) {
        extrapolationPoint->create(l_helixPoint, l_corehit);
        break;
      }
    }
  }




  return StatusCode::SUCCESS;
}

StatusCode RecHelixTrajectory::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
