
#include "DetInterface/ITrackingGeoSvc.h"
#include "DetInterface/ITrkGeoDumpSvc.h"
#include "DetInterface/IGeoSvc.h"

#include "datamodel/TrackHitCollection.h"

#include "ACTS/Utilities/Identifier.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/EventData/Measurement.hpp"
#include "ACTS/Examples/BuildGenericDetector.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/Fitter/KalmanFitter.hpp"
#include "ACTS/Fitter/KalmanUpdator.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/Surfaces/PerigeeSurface.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"



#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDSegmentation/BitField64.h"
#include "DDRec/API/IDDecoder.h"

#include <cmath>

#include "TrackFit.h"



DECLARE_ALGORITHM_FACTORY(TrackFit)

TrackFit::TrackFit(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareInput("positionedTrackHits", m_positionedTrackHits,"hits/TrackerPositionedHits");

  }

TrackFit::~TrackFit() {}

StatusCode TrackFit::initialize() {

  m_geoSvc = service ("GeoSvc");

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_trkGeoSvc = service("TrackingGeoSvc");
  if (nullptr == m_trkGeoSvc) {
    error() << "Unable to locate Tracking Geometry Service. " << endmsg;
    return StatusCode::FAILURE;
  }

  m_trkGeoDumpSvc = service("TrkGeoDump");
  if (nullptr == m_trkGeoDumpSvc) {
    error() << "Unable to locate Tracking Geometry Service. " << endmsg;
    return StatusCode::FAILURE;
  }

  m_trkGeo = m_trkGeoSvc->trackingGeometry();
  return sc;
}

StatusCode TrackFit::execute() {

    auto lcdd = m_geoSvc->lcdd();
    //DD4hep::Geometry::VolumeManager volman = lcdd->volumeManager();
    //DD4hep::DDRec::IDDecoder& iddec = DD4hep::DDRec::IDDecoder::getInstance();
    auto allReadouts = lcdd->readouts();
    auto readoutBarrel = lcdd->readout("TrackerBarrelReadout");
    //auto readoutEndcap = lcdd->readout("TrackerEndcapReadout");
    auto m_decoderBarrel = readoutBarrel.idSpec().decoder();
    /*
    auto segmentationBarrel = readoutBarrel.segmentation();
    info() << "Barrel segmentation of type " << segmentationBarrel->type() << endmsg;
    auto m_decoderEndcap = readoutEndcap.idSpec().decoder();
    auto segmentationEndcap = readoutEndcap.segmentation().segmentation();
    */

  const fcc::PositionedTrackHitCollection* hits = m_positionedTrackHits.get();

  std::vector<FitMeas_t> fccMeasurements;
  std::vector<const Acts::Surface*> surfVec;
  fccMeasurements.reserve(hits->size());

  /// @todo: get local position from segmentation
  double fcc_l1 = 0;
  double fcc_l2 = 0;
  for (auto hit: *hits) {
    const Acts::Surface* fccSurf = m_trkGeoDumpSvc->lookUpTrkSurface(Identifier(hit.core().cellId));
    double std1 = 1;
    double std2 = 1;
    ActsSymMatrixD<2> cov;
    cov << std1* std1, 0, 0, std2* std2;
    fccMeasurements.push_back(Meas_t<eLOC_1, eLOC_2>(*fccSurf, hit.core().cellId, std::move(cov), fcc_l1, fcc_l2));
    surfVec.push_back(fccSurf);

    // debug printouts
    long long int theCellId = hit.core().cellId;
    std::cout << theCellId << std::endl;
    std::cout << "surface pointer: " << fccSurf<< std::endl;
    std::cout << "position: x: " << hit.position().x << "\t y: " << hit.position().y << "\t z: " << hit.position().z << std::endl; 
    std::cout << "phi: " << std::atan(hit.position().y / hit.position().x) << std::endl;
    m_decoderBarrel->setValue(theCellId);
    int system_id = (*m_decoderBarrel)["system"];
    std::cout << " hit in system: " << system_id;
    if ( 14 == system_id ) {
      int layer_id = (*m_decoderBarrel)["layer"];
      std::cout << "\t layer " << layer_id;
      int module_id = (*m_decoderBarrel)["module"];
      std::cout << "\t module " << module_id;
      int component_id = (*m_decoderBarrel)["component"];
      std::cout << "\t component " << component_id;
      std::cout << std::endl;
    }
  }

  ActsVector<ParValue_t, NGlobalPars> pars;
  pars << 0, 0, 0, M_PI / 2, 0.0001;
  auto startCov =
      std::make_unique<ActsSymMatrix<ParValue_t, NGlobalPars>>(ActsSymMatrix<ParValue_t, NGlobalPars>::Identity());

  const Surface* pSurf =  m_trkGeo->getBeamline();
  auto startTP = std::make_unique<BoundParameters>(std::move(startCov), std::move(pars), *pSurf);

  ExtrapolationCell<TrackParameters> exCell(*startTP);
  exCell.addConfigurationMode(ExtrapolationMode::CollectSensitive);
  exCell.addConfigurationMode(ExtrapolationMode::CollectPassive);
  exCell.addConfigurationMode(ExtrapolationMode::StopAtBoundary);

  auto exEngine = initExtrapolator(m_trkGeo);
  exEngine->extrapolate(exCell);

  debug() << "got " << exCell.extrapolationSteps.size() << " extrapolation steps" << endmsg;

  std::vector<FitMeas_t> vMeasurements;
  vMeasurements.reserve(exCell.extrapolationSteps.size());

  // identifier
  long int id = 0;
  // random numbers for smearing measurements
  /// @todo: use random number service
  std::default_random_engine e;
  std::uniform_real_distribution<double> std_loc1(1, 5);
  std::uniform_real_distribution<double> std_loc2(0.1, 2);
  std::normal_distribution<double> g(0, 1);

  double std1, std2, l1, l2;
  for (const auto& step : exCell.extrapolationSteps) {
    const auto& tp = step.parameters;
    if (tp->associatedSurface().type() != Surface::Plane) continue;

    std1 = 1;//std_loc1(e);
    std2 = 1; //std_loc2(e);
    l1 = tp->get<eLOC_1>(); // + std1 * g(e);
    l2 = tp->get<eLOC_2>(); // + std2 * g(e);
    ActsSymMatrixD<2> cov;
    cov << std1* std1, 0, 0, std2* std2;
    vMeasurements.push_back(Meas_t<eLOC_1, eLOC_2>(tp->associatedSurface(), id, std::move(cov), l1, l2));
    ++id;
  }

  debug() << "created " << vMeasurements.size() << " pseudo-measurements" << endmsg;
  for (const auto& m : vMeasurements)
    info() << m << std::endl
           << endmsg;

  debug() << "created " << fccMeasurements.size() << " fcc-measurements" << endmsg;
  for (const auto& m : fccMeasurements)
    info() << m << std::endl
           << endmsg;

  KalmanFitter<MyExtrapolator, CacheGenerator, NoCalibration, GainMatrixUpdator> KF;
  KF.m_oCacheGenerator = CacheGenerator();
  KF.m_oCalibrator = NoCalibration();
  KF.m_oExtrapolator = MyExtrapolator(exEngine);
  KF.m_oUpdator = GainMatrixUpdator();

  auto track = KF.fit(fccMeasurements, std::make_unique<BoundParameters>(*startTP));

  // dump track
  for (const auto& p : track) {
    debug() << *p->getCalibratedMeasurement() << endmsg;
    debug() << *p->getSmoothedState() << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrackFit::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
