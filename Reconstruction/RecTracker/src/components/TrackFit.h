#ifndef RECTRACKER_TRACKFIT_H
#define RECTRACKER_TRACKFIT_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;
class ITrackingGeoSvc;
class ITrkVolumeManagerSvc;
class ITrackSeedingTool;


namespace Acts {
class TrackingGeometry;
}

namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}

class TrackFit : public GaudiAlgorithm {
public:
  TrackFit(const std::string& name, ISvcLocator* svcLoc);

  ~TrackFit();

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Pointer to the geometry service
  SmartIF<ITrackingGeoSvc> m_trkGeoSvc;
  SmartIF<ITrkVolumeManagerSvc> m_trkVolMan;
  SmartIF<IGeoSvc> m_geoSvc;

  ToolHandle<ITrackSeedingTool> m_trackSeedingTool;

  std::shared_ptr<Acts::TrackingGeometry> m_trkGeo;

  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits;
};

using namespace Acts;

typedef FittableMeasurement<long int> FitMeas_t;
template <ParID_t... pars>
using Meas_t = Measurement<long int, pars...>;

struct MyCache {
  std::unique_ptr<const KF::Step<long int>::JacobianMatrix> jacobian;
  std::unique_ptr<const BoundParameters> parameters;

  MyCache() = default;
  MyCache(const MyCache&) = delete;
  MyCache(MyCache&&) = default;
};

class MyExtrapolator {
public:
  MyExtrapolator(std::shared_ptr<const IExtrapolationEngine> exEngine = nullptr) : m_exEngine(std::move(exEngine)) {}

  MyCache operator()(const FitMeas_t& m, const TrackParameters& tp) const {
    auto exCell = std::make_unique<ExtrapolationCell<TrackParameters>>(tp);
    //exCell->addConfigurationMode(ExtrapolationMode::Destination);
    exCell->addConfigurationMode(ExtrapolationMode::FATRAS);
    exCell->addConfigurationMode(ExtrapolationMode::CollectJacobians);
    const Surface& sf = getSurface(m);

    m_exEngine->extrapolate(*exCell, &sf);
    MyCache c;
    auto j = exCell->extrapolationSteps.back().transportJacobian.release();
    c.jacobian.reset(new KF::Step<long int>::JacobianMatrix(*j));
    auto pars = static_cast<const BoundParameters*>(exCell->leadParameters->clone());
    c.parameters.reset(pars);

    return c;
  }

private:
  std::shared_ptr<const IExtrapolationEngine> m_exEngine;
};

class NoCalibration {
public:
  std::unique_ptr<const FitMeas_t> operator()(const FitMeas_t& m, const BoundParameters&) const {
    return std::make_unique<const FitMeas_t>(m);
  }
};

class CacheGenerator {
public:
  std::unique_ptr<KF::Step<long int>> operator()(MyCache m) const {
    auto step = std::make_unique<KF::Step<long int>>();
    step->setPredictedState(std::move(m.parameters));
    step->setJacobian(std::move(m.jacobian));

    return step;
  }
};

std::shared_ptr<IExtrapolationEngine> initExtrapolator(const std::shared_ptr<const TrackingGeometry>& geo) {
  auto propConfig = RungeKuttaEngine<>::Config();
  /// @todo: use magnetic field service
  propConfig.fieldService = std::make_shared<ConstantBField>(0, 0, 0.002);
  auto propEngine = std::make_shared<RungeKuttaEngine<>>(propConfig);

  auto matConfig = MaterialEffectsEngine::Config();
  auto materialEngine = std::make_shared<MaterialEffectsEngine>(matConfig);

  auto navConfig = StaticNavigationEngine::Config();
  navConfig.propagationEngine = propEngine;
  navConfig.materialEffectsEngine = materialEngine;
  navConfig.trackingGeometry = geo;
  auto navEngine = std::make_shared<StaticNavigationEngine>(navConfig);

  auto statConfig = StaticEngine::Config();
  statConfig.propagationEngine = propEngine;
  statConfig.navigationEngine = navEngine;
  statConfig.materialEffectsEngine = materialEngine;
  auto statEngine = std::make_shared<StaticEngine>(statConfig);

  auto exEngineConfig = ExtrapolationEngine::Config();
  exEngineConfig.trackingGeometry = geo;
  exEngineConfig.propagationEngine = propEngine;
  exEngineConfig.navigationEngine = navEngine;
  exEngineConfig.extrapolationEngines = {statEngine};
  auto exEngine = std::make_shared<ExtrapolationEngine>(exEngineConfig);

  return exEngine;
}

#endif /* RECTRACKER_TRACKFIT_H */
