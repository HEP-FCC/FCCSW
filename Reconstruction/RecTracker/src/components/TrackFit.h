#ifndef RECTRACKER_TRACKFIT_H
#define RECTRACKER_TRACKFIT_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "ACTSLogger.h"

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


typedef Acts::FittableMeasurement<long int> FitMeas_t;
template <Acts::ParID_t... pars>
using Meas_t = Acts::Measurement<long int, pars...>;

struct MyCache {
  std::unique_ptr<const Acts::KF::Step<long int>::JacobianMatrix> jacobian;
  std::unique_ptr<const Acts::BoundParameters> parameters;

  MyCache() = default;
  MyCache(const MyCache&) = delete;
  MyCache(MyCache&&) = default;
};

class MyExtrapolator {
public:
  MyExtrapolator(std::shared_ptr<const Acts::IExtrapolationEngine> exEngine = nullptr) : m_exEngine(std::move(exEngine)) {}

  MyCache operator()(const FitMeas_t& m, const Acts::TrackParameters& tp) const {
    auto exCell = std::make_unique<Acts::ExtrapolationCell<Acts::TrackParameters>>(tp);
    //exCell->addConfigurationMode(ExtrapolationMode::Destination);
    exCell->addConfigurationMode(Acts::ExtrapolationMode::FATRAS);
    exCell->addConfigurationMode(Acts::ExtrapolationMode::CollectJacobians);
    const Acts::Surface& sf = getSurface(m);

    m_exEngine->extrapolate(*exCell, &sf);
    MyCache c;
    auto j = exCell->extrapolationSteps.back().transportJacobian.release();
    c.jacobian.reset(new Acts::KF::Step<long int>::JacobianMatrix(*j));
    auto pars = static_cast<const Acts::BoundParameters*>(exCell->leadParameters->clone());
    c.parameters.reset(pars);

    return c;
  }

private:
  std::shared_ptr<const Acts::IExtrapolationEngine> m_exEngine;
};

class NoCalibration {
public:
  std::unique_ptr<const FitMeas_t> operator()(const FitMeas_t& m, const Acts::BoundParameters&) const {
    return std::make_unique<const FitMeas_t>(m);
  }
};

class CacheGenerator {
public:
  std::unique_ptr<Acts::KF::Step<long int>> operator()(MyCache m) const {
    auto step = std::make_unique<Acts::KF::Step<long int>>();
    step->setPredictedState(std::move(m.parameters));
    step->setJacobian(std::move(m.jacobian));

    return step;
  }
};

std::shared_ptr<Acts::IExtrapolationEngine> initExtrapolator(const std::shared_ptr<const Acts::TrackingGeometry>& geo) {
  auto propConfig = Acts::RungeKuttaEngine<>::Config();
  /// @todo: use magnetic field service
  propConfig.fieldService = std::make_shared<Acts::ConstantBField>(0, 0, 0.002); // kT
  auto propEngine = std::make_shared<Acts::RungeKuttaEngine<>>(propConfig);

  auto matConfig = Acts::MaterialEffectsEngine::Config();
  auto materialEngine = std::make_shared<Acts::MaterialEffectsEngine>(matConfig);

  auto navConfig = Acts::StaticNavigationEngine::Config();
  navConfig.propagationEngine = propEngine;
  navConfig.materialEffectsEngine = materialEngine;
  navConfig.trackingGeometry = geo;
  auto navEngine = std::make_shared<Acts::StaticNavigationEngine>(navConfig);

  auto statConfig = Acts::StaticEngine::Config();
  statConfig.propagationEngine = propEngine;
  statConfig.navigationEngine = navEngine;
  statConfig.materialEffectsEngine = materialEngine;
  auto statEngine = std::make_shared<Acts::StaticEngine>(statConfig);

  auto exEngineConfig = Acts::ExtrapolationEngine::Config();
  exEngineConfig.trackingGeometry = geo;
  exEngineConfig.propagationEngine = propEngine;
  exEngineConfig.navigationEngine = navEngine;
  exEngineConfig.extrapolationEngines = {statEngine};
  auto exEngine = std::make_shared<Acts::ExtrapolationEngine>(exEngineConfig);
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "Expol");
  auto GaudiLogger = std::make_unique<Acts::Logger>(std::make_unique<GaudiPrintPolicy>(&(*msgSvc)),std::make_unique<GaudiFilterPolicy>(&(*msgSvc)));
  exEngine->setLogger( std::move(GaudiLogger));

  return exEngine;
}

#endif /* RECTRACKER_TRACKFIT_H */
