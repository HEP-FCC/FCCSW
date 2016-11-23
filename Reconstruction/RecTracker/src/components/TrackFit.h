#ifndef RECTRACKER_TRACKFIT_H
#define RECTRACKER_TRACKFIT_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;
class ITrackingGeoSvc;

namespace Acts {
class TrackingGeometry;
}

namespace fcc {
class TrackHitCollection;
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
  std::shared_ptr<Acts::TrackingGeometry> m_trkGeo;
};

#endif /* RECTRACKER_TRACKFIT_H */
