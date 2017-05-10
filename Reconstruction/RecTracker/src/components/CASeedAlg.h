#ifndef RECTRACKER_CASEEDALG_H
#define RECTRACKER_CASEEDALG_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/SystemOfUnits.h"

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
  Gaudi::Property<double> m_deltaPhi{this, "deltaPhi", 1.5 * Gaudi::Units::rad, "KDTree search window in Phi"};
  Gaudi::Property<double> m_deltaZ{this, "deltaZ", 20 * Gaudi::Units::mm, "KDTree search window in Z"};
  Gaudi::Property<double> m_deltaT{this, "deltaT", 50 * Gaudi::Units::ns, "KDTree search window in T"};
};

#endif /* RECTRACKER_CASEEDALG_H */
