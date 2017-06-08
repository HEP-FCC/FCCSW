#ifndef RECTRACKER_EXTRAPOLATIONTEST_H
#define RECTRACKER_EXTRAPOLATIONTEST_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/RndmGenerators.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;
class ITrackingGeoSvc;
class ITrkGeoDumpSvc;
class ITrackSeedingTool;

namespace Acts {
class TrackingGeometry;
class IExtrapolationEngine;
}

namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}

class ExtrapolationTest : public GaudiAlgorithm {
public:
  ExtrapolationTest(const std::string& name, ISvcLocator* svcLoc);

  ~ExtrapolationTest() = default;

  StatusCode initialize() override final;

  StatusCode execute() override final;

  StatusCode finalize() override final;

private:
  /// Pointer to the geometry service
  SmartIF<ITrackingGeoSvc> m_trkGeoSvc;
  SmartIF<IGeoSvc> m_geoSvc;

  std::shared_ptr<Acts::TrackingGeometry> m_trkGeo;
  std::shared_ptr<const Acts::IExtrapolationEngine> m_exEngine;

  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedHits", Gaudi::DataHandle::Writer,
                                                                      this};
  Gaudi::Property<float> m_magneticFieldBz{this, "magneticFieldBz", 4., "const magnetic field strength in z"};
  Rndm::Numbers m_flatDist;
};


typedef Acts::FittableMeasurement<long int> FitMeas_t;
template <Acts::ParID_t... pars>
using Meas_t = Acts::Measurement<long int, pars...>;

#endif /* RECTRACKER_EXTRAPOLATIONTEST_H */
