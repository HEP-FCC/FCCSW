
#include "DetInterface/IGeoSvc.h"

#include "GaudiKernel/PhysicalConstants.h"

#include "edm4hep/TrackerHitCollection.h"
#include "edm4hep/TrackCollection.h"

#include <cmath>
#include <random>

#include "RecHelixTrajectory.h"
#include "RecTracker/TrackingUtils.h"

DECLARE_COMPONENT(RecHelixTrajectory)

RecHelixTrajectory::RecHelixTrajectory(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("RecHelixPoints", m_recHelixPoints, "Tracker hits (Output)");
  declareProperty("Tracks", m_tracks, "TrackStates (Input)");
}

StatusCode RecHelixTrajectory::initialize() {
  debug() << "initialize" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode RecHelixTrajectory::execute() {

  // get the track states to turn into helices from event store
  const edm4hep::TrackCollection* tracks = m_tracks.get();

  edm4hep::TrackerHitCollection* points = m_recHelixPoints.createAndPut();

  unsigned int maxSteps = m_maxPathLength / m_stepSize;
  for (auto track : (*tracks)) {
    auto trackState = track.getTrackStates(0);
    double q_pT = trackState.Omega; 
    double cotTheta = trackState.tanLambda;
    double phi0 = trackState.phi;
    double d0 = trackState.D0;
    double z0 = trackState.Z0;

    double charge = (q_pT > 0) - (q_pT < 0);  // get charge from sign of q over pt
    double rho = charge / std::abs(q_pT) / 0.0003 / 4.;
    double xc = (rho - d0) * std::cos(phi0 + 0.5 * M_PI);
    double yc = (rho - d0) * std::sin(phi0 + 0.5 * M_PI);
    for (double i = 0; i < maxSteps; i += m_stepSize) {
      edm4hep::TrackerHit theHit;

      double l_helixPoint_x = xc + rho * std::cos(charge * i + phi0 - M_PI * 0.5),
      double l_helixPoint_y = yc + rho * std::sin(charge * i + phi0 - M_PI * 0.5),
      double l_helixPoint_z = z0 + cotTheta * std::abs(rho) * i;
      theHit.setPosition({
           l_helixPoint_x,
           l_helixPoint_y,
           l_helixPoint_z,
           });
      points->push_back(theHit);
      if (std::abs(l_helixPoint_z) > m_maxZ) {
        break;
      }
      if (std::sqrt(std::pow(l_helixPoint_x, 2) + std::pow(l_helixPoint_y, 2)) > m_maxR) {
        break;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode RecHelixTrajectory::finalize() { return GaudiAlgorithm::finalize(); }
