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

  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedTrackHits", Gaudi::DataHandle::Reader, this};
  Gaudi::Property<double> m_deltaPhi{this, "deltaPhi", 1.5, "KDTree search window in Phi"};
  Gaudi::Property<double> m_deltaZ{this, "deltaZ", 1.5, "KDTree search window in Z"};
  Gaudi::Property<double> m_deltaT{this, "deltaT", 1.5, "KDTree search window in T"};
};

#endif /* RECTRACKER_CASEEDALG_H */
