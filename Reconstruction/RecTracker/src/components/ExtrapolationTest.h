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
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Tracking geometry service
  ServiceHandle<ITrackingGeoSvc> m_trkGeoSvc;

  /// pointer to tracking geometry world
  std::shared_ptr<Acts::TrackingGeometry> m_trkGeo;
  /// pointer to Acts extrapolation engine
  std::shared_ptr<const Acts::IExtrapolationEngine> m_exEngine;

  /// output of points along track
  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedHits", Gaudi::DataHandle::Writer,
                                                                      this};
  Gaudi::Property<float> m_magneticFieldBz{this, "magneticFieldBz", 4., "const magnetic field strength in z"};
  /// Random number generator for the track parameters
  Rndm::Numbers m_flatDist;
};


typedef Acts::FittableMeasurement<long int> FitMeas_t;
template <Acts::ParID_t... pars>
using Meas_t = Acts::Measurement<long int, pars...>;

#endif /* RECTRACKER_EXTRAPOLATIONTEST_H */
