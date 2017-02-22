
#include "DetInterface/ITrackingGeoSvc.h"
#include "RecInterface/ITrkVolumeManagerSvc.h"
#include "DetInterface/IGeoSvc.h"
#include "RecInterface/ITrackSeedingTool.h"

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
#include "FastHelix.h"
#include "TrackParameterConversions.h"

#include "ACTSLogger.h"



DECLARE_ALGORITHM_FACTORY(TrackFit)

TrackFit::TrackFit(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareInput("positionedTrackHits", m_positionedTrackHits,"hits/TrackerPositionedHits");
  declareProperty("trackSeedingTool", m_trackSeedingTool);
  declarePublicTool(m_trackSeedingTool, "TrackSeedingTool/TruthSeedingTool");

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

  m_trkVolMan = service("TrkVolMan");
  if (nullptr == m_trkVolMan) {
    error() << "Unable to locate Tracking Geometry Service. " << endmsg;
    return StatusCode::FAILURE;
  }

  m_trkGeo = m_trkGeoSvc->trackingGeometry();

  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "TrackFit");
  MsgStream lLog(&(*msgSvc), "HCalConstruction");
  auto GaudiLogger = std::make_unique<Acts::Logger>(std::make_unique<GaudiPrintPolicy>(&(*msgSvc)),std::make_unique<GaudiFilterPolicy>(&(*msgSvc)));
  return sc;
}

StatusCode TrackFit::execute() {

    auto lcdd = m_geoSvc->lcdd();
    auto allReadouts = lcdd->readouts();
    auto readoutBarrel = lcdd->readout("TrackerBarrelReadout");
    auto m_decoderBarrel = readoutBarrel.idSpec().decoder();

  const fcc::PositionedTrackHitCollection* hits = m_positionedTrackHits.get();

  std::vector<FitMeas_t> fccMeasurements;
  std::vector<const Acts::Surface*> surfVec;
  fccMeasurements.reserve(hits->size());

  /// @todo: get local position from segmentation
  double fcc_l1 = 0;
  double fcc_l2 = 0;
  int hitcounter = 0;
  GlobalPoint middlePoint;
  GlobalPoint outerPoint;

  auto seedmap = m_trackSeedingTool->findSeeds(hits);
  for (auto it = seedmap.begin(); it != seedmap.end(); ++it) {
    if (it->first == 1) { // TrackID for primary particle

    auto hit = (*hits)[it->second]; // the TrackID maps to an index for the hit collection

    long long int theCellId = hit.core().cellId;
    debug() << theCellId << endmsg;
    debug() << "position: x: " << hit.position().x << "\t y: " << hit.position().y << "\t z: " << hit.position().z << endmsg; 
    debug() << "phi: " << std::atan2(hit.position().y, hit.position().x) << endmsg;
    m_decoderBarrel->setValue(theCellId);
    int system_id = (*m_decoderBarrel)["system"];
    debug() << " hit in system: " << system_id;
    int layer_id = (*m_decoderBarrel)["layer"];
    debug() << "\t layer " << layer_id;
    int module_id = (*m_decoderBarrel)["module"];
    debug() << "\t module " << module_id;
    debug() << endmsg;
    (*m_decoderBarrel)["x"] = 0; // workaround for broken `volumeID` method --
    (*m_decoderBarrel)["z"] = 0; // set everything not connected with the VolumeID to zero,
    // so the cellID can be used to look up the tracking surface
    if (hitcounter == 0) {
      middlePoint = GlobalPoint(hit.position().x, hit.position().y, hit.position().z);
    } else if (hitcounter == 1) {

      outerPoint = GlobalPoint(hit.position().x, hit.position().y, hit.position().z);
    }
    // need to use cellID without segmentation bits
    const Acts::Surface* fccSurf = m_trkVolMan->lookUpTrkSurface(Identifier(m_decoderBarrel->getValue()));
    debug() << " found surface pointer: " << fccSurf<< endmsg;
    if (nullptr != fccSurf) {
    double std1 = 1;
    double std2 = 1;
    ActsSymMatrixD<2> cov;
    cov << std1* std1, 0, 0, std2* std2;
    fccMeasurements.push_back(Meas_t<eLOC_1, eLOC_2>(*fccSurf, hit.core().cellId, std::move(cov), fcc_l1, fcc_l2));
    surfVec.push_back(fccSurf);

  // debug printouts
    
    ++hitcounter;
    }
    }
  }

  
  FastHelix helix(outerPoint, middlePoint, GlobalPoint(0,0,0), 0.002);
  PerigeeTrackParameters res = ParticleProperties2TrackParameters(GlobalPoint(0,0,0), helix.getPt(), 0.002, 1);
  

  ActsVector<ParValue_t, NGlobalPars> pars;
  pars << res.d0, res.z0, res.phi0, M_PI * 0.5, res.qOverPt;
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
    info() << m << endmsg;

  debug() << "created " << fccMeasurements.size() << " fcc-measurements" << endmsg;
  for (const auto& m : fccMeasurements)
    info() << m << endmsg;

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
