#ifndef RECTRACKER_CASEEDALG_H
#define RECTRACKER_CASEEDALG_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;
class ITrackingGeoSvc;

namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}

class CASeedAlg : public GaudiAlgorithm {
public:
  CASeedAlg(const std::string& name, ISvcLocator* svcLoc);

  ~CASeedAlg();

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;

  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits;
};

#endif /* RECTRACKER_CASEEDALG_H */
