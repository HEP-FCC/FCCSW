
#include "ActsExtrapolationTool.h"

#include "Acts/Surfaces/PerigeeSurface.hpp"

#include "DetInterface/IGeoSvc.h"
#include "DetInterface/ITrackingGeoSvc.h"
#include "RecInterface/ITrackSeedingTool.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "DD4hep/Detector.h"

#include <cmath>
#include <random>

#include <Eigen/Dense>

#include "KalmanFilter.h"

/// Holds the result (predicted track state + measurement) of the
/// prediciton step
struct PredictionResult {
  Eigen::Matrix<double, 5, 1> trackParameters;
  Eigen::Matrix<double, 5, 5> trackCovariance;
  const Acts::Surface* trackSurface;
  Eigen::Matrix<double, 2, 1> measurement;
  Eigen::Matrix<double, 2, 2> measurementCovariance;
  bool extrapolationSucceeded;
};

DECLARE_ALGORITHM_FACTORY(KalmanFilter)

KalmanFilter::KalmanFilter(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc), m_extrapolationTool(nullptr) {
  declareProperty("extrapolationTool", m_extrapolationTool,
                  "Pointer to extrapolation tool, needed to extrapolate through the tracker.");
  declareProperty("FittedTracks", m_fittedTracks, "tracks/FittedTracks");
  declareProperty("TrackSeeds", m_trackSeeds, "tracks/TrackSeeds");
  declareProperty("TrackerHits", m_hits, "TrackerHits");
}

StatusCode KalmanFilter::initialize() {
  if (!m_extrapolationTool.retrieve()) {
    error() << "Extrapolation tool cannot be retrieved" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode KalmanFilter::execute() {

  /// Input, initial state
  const fcc::TrackStateCollection* trackSeeds = m_trackSeeds.get();
  const fcc::PositionedTrackHitCollection* trackHits = m_hits.get();

  fcc::TrackState theTrackState = (*trackSeeds)[0];

  auto predict = [this, &trackHits](Acts::BoundParameters startParameters) {
    auto ecc = this->m_extrapolationTool->getExtrapolationCell(startParameters);
    int stepCounter = 0;
    for (const auto& step : ecc.extrapolationSteps) {
      const auto& tp = step.parameters;
      if (tp) {
        if (step.surface->associatedDetectorElement()) {
          std::cout << "acts covariance: " << std::endl;
          std::cout << *(tp->covariance()) << std::endl;
          std::cout << "acts full parameters" << std::endl;
          std::cout << tp->parameters() << std::endl;
          std::cout << "global pos: " << std::endl;
          std::cout << tp->position() << std::endl;
          // select  closest measurement
          int trackHitIndex = 0;
          int indexBestMeas = 0;
          double minDist = 1e12;
          for (const auto& hit : *trackHits) {
            auto _mp = hit.position();
            auto _tp = tp->position();
            double dist =
                std::sqrt(std::pow(_mp.x - _tp.x(), 2) + std::pow(_mp.y - _tp.y(), 2) + std::pow(_mp.z - _tp.z(), 2));
            if (dist < minDist) {
              minDist = dist;
              indexBestMeas = trackHitIndex;
            }
            trackHitIndex++;
          }
          std::cout << "best meas: " << indexBestMeas << std::endl;
          Acts::Vector2D localPos;
          Acts::Vector3D globalPos((*trackHits)[indexBestMeas].position().x, (*trackHits)[indexBestMeas].position().y,
                                   (*trackHits)[indexBestMeas].position().z);
          std::cout << "testing global2local..." << std::endl;
          step.surface->globalToLocal(globalPos, tp->momentum(), localPos);
          std::cout << localPos << std::endl;
          /// return PredictionResult
          auto result = PredictionResult();
          result.trackParameters = tp->parameters();
          result.trackCovariance = *tp->covariance();
          result.trackSurface = step.surface;
          result.measurement = localPos;
          result.extrapolationSucceeded = true;
          if (stepCounter > 1) {
            return result;
          }
          stepCounter++;
        }
      }  // if track parameters
    }
    // in case extrapolation wasn't succesful
    auto failResult = PredictionResult();
    failResult.extrapolationSucceeded = false;
    return failResult;
  };

  ///////////////// converting fcc measurements to fcc measurements
  /*
  dd4hep::DDSegmentation::BitField64* bf = new
  dd4hep::DDSegmentation::BitField64("system:4,layer:5,module:18,x:-15,z:-15");
  for (const auto& hit: *trackHits) {
    bf->setValue(hit.cellId());
    std::cout << "fccsw global pos: " << "\t" << hit.position().x << "\t" << hit.position().y << "\t" <<
  hit.position().z << std::endl;
    double fcc2ActsM_x = 1;
    double fcc2ActsM_y = -1;
    if ((*bf)["system"] < 2) {
      fcc2ActsM_y = -1;
    } else {
      fcc2ActsM_y = 1;
    }
    std::cout << "fccsw local pos: " << "\t" << (*bf)["x"] * 0.005 * fcc2ActsM_x  << "\t" << (*bf)["z"] * 0.01  *
  fcc2ActsM_y << std::endl;
  }

  for (const auto& trackSeed: *trackSeeds) {
    std::cout << "acts global pos: " <<"\t" << trackSeed.referencePoint().x << "\t" << trackSeed.referencePoint().y <<
  "\t" << trackSeed.referencePoint().z << std::endl;
    std::cout << "acts local pos: " << "\t" << trackSeed.d0() << "\t" << trackSeed.z0() << std::endl;
  }
  */

  std::list<std::pair<Eigen::Matrix<double, 5, 1>, Eigen::Matrix<double, 5, 5>>> filtered_states;
  std::list<std::pair<Eigen::Matrix<double, 5, 1>, Eigen::Matrix<double, 5, 5>>> smoothed_states;
  std::list<std::pair<Eigen::Matrix<double, 5, 1>, Eigen::Matrix<double, 5, 5>>> predicted_states;

  // A) Forward Filter

  // outsourcing the selection of new measurements
  // the idea is that the next measurement can be selected in a number of ways
  // so the fitter needs an interface that returns measurements as long as there are more steps to take
  // also effectively decouples the fitter logic from the extrapolation and geometry

  // prepare some dummy measurements and covariances
  /*
  std::vector<Eigen::Matrix<double, 2, 1>> measurement_candidates;
  std::vector<Eigen::Matrix<double, 2, 2>> measurement_candidates_cov;
  for (int i = 0; i < 10; ++i) {
    measurement_candidates.push_back(Eigen::Matrix<double, 2 , 1>::Random());
    measurement_candidates_cov.push_back(Eigen::Matrix<double, 2 , 2>::Random());
  }


  // dummy next_measurement function
  int currentStep = 0;
  auto get_next_measurement = [&measurement_candidates, &measurement_candidates_cov, &currentStep]() {
    if (currentStep < measurement_candidates.size()) {
       auto _m = measurement_candidates[currentStep];
       auto _m_cov = measurement_candidates_cov[currentStep];
       currentStep++;
       return std::make_pair(_m, _m_cov);
    } else {
      Eigen::Matrix<double, 2, 1> m;
      m << std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN();
      return std::make_pair(m, Eigen::Matrix<double, 2, 2>());
    }
    };
  // helper function
  auto  is_nan = [](const Eigen::Matrix<double, 2, 1>& x)
  {
    return !((x.array() == x.array())).all();
  };
  */

  // 2) Find next measurement
  //   * this could be from a given pattern recognition
  //   * or be used iteratively with a number of candidates

  // 3) Gain Matrix Update

  // get matrix H (projector)
  // in the default fcc hh usecase, H just selects the two local coordinates ( eloc0 and eloc1 )
  // H *  covariance * H.transpose()  selects just the submatrix of the

  Eigen::Matrix<double, 2, 5> H;

  H.row(0) << 1, 0, 0, 0, 0;
  H.row(1) << 0, 1, 0, 0, 0;
  std::cout << "H" << std::endl;
  std::cout << H << std::endl;

  /////////    prediction + next measurement step /////
  Eigen::Matrix<double, 5, 1> predictedState;
  auto refPoint = theTrackState.referencePoint();
  Acts::Vector3D perigee(refPoint.x, refPoint.y, refPoint.z);
  Acts::PerigeeSurface surface(perigee);
  double d0 = theTrackState.d0();
  double z0 = theTrackState.z0();
  double phi = theTrackState.phi();
  double theta = theTrackState.theta();
  double qop = theTrackState.qOverP();
  // parameters
  predictedState << d0, z0, phi, theta, qop;
  std::unique_ptr<Acts::ActsSymMatrixD<5>> cov =
      std::make_unique<Acts::ActsSymMatrixD<5>>(0.00001 * Acts::ActsSymMatrixD<5>::Identity());
  // create the bound parameters
  Acts::BoundParameters startParameters(std::move(cov), std::move(predictedState), surface);

  while (1) {

    auto res = predict(startParameters);
    if (!res.extrapolationSucceeded) {
      break;
    }
    predicted_states.push_back(std::make_pair(res.trackParameters, res.trackCovariance));

    /// m ... measurement in local coordinates
    auto m = res.measurement;
    /// V ... measurement covariance
    auto V = res.measurementCovariance;
    ///  p ... track parameters
    auto p = res.trackParameters;
    /// C ... track parameter covariance
    auto C = res.trackCovariance;
    /// K ... Gain matrix
    Eigen::Matrix<double, 5, 2> K = C * H.transpose() * (V + H * C * H.transpose()).inverse();
    /// shorthand for identity matrix
    static const Eigen::Matrix<double, 5, 5> unit = Eigen::Matrix<double, 5, 5>::Identity();
    /// filtered track parameters
    Eigen::Matrix<double, 5, 1> new_p = p + K * (m - H * p);
    /// filtered track covariances
    Eigen::Matrix<double, 5, 5> new_C = (unit - K * H) * C;
    // create ACTS bound paramters from result (will be used for next extrapolation)
    std::unique_ptr<Acts::ActsSymMatrixD<5>> new_C_ptr = std::make_unique<Acts::ActsSymMatrixD<5>>(new_C);
    startParameters = Acts::BoundParameters(std::move(new_C_ptr), std::move(new_p), *res.trackSurface);

    // book-keeping
    filtered_states.push_back(std::make_pair(new_p, new_C));
  }

  debug() << "number of filtered states: " << filtered_states.size() << endmsg;

  // B Smoothing
  auto it = filtered_states.rbegin();

  // for the last measurement the filtered state and the smoothed state are
  // equal

  auto p_previousStep_smoothed = (*it).first;
  auto C_previousStep_smoothed = (*it).second;

  decltype(it) pLast = it++;
  unsigned int backCounter = 0;
  for (; it != filtered_states.rend(); ++it, ++pLast) {

    /// F ... Transport Jacobian
    Eigen::Matrix<double, 5, 5> F = Eigen::Matrix<double, 5, 5>::Random();

    auto p = (*it).first;
    auto p_previousStep = (*pLast).first;

    auto C = (*it).second;
    auto C_previousStep = (*pLast).second;

    /// A
    auto A = C * F * C_previousStep.inverse();

    auto p_smoothed = p - A * (p_previousStep - p_previousStep_smoothed);

    auto C_smoothed = C - A * (C_previousStep - C_previousStep_smoothed) * A.transpose();
    smoothed_states.push_back(std::make_pair(p_smoothed, C_smoothed));
    p_previousStep_smoothed = p_smoothed;
    C_previousStep_smoothed = C_smoothed;

    backCounter++;
  }

  // output
  auto fittedTrackCollection = m_fittedTracks.createAndPut();

  return StatusCode::SUCCESS;
}

StatusCode KalmanFilter::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
