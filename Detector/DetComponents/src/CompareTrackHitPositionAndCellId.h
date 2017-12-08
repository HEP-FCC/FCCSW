#ifndef DETCOMPONENTS_COMPARETRACKHITPOSITIONSANDCELLID_H
#define DETCOMPONENTS_COMPARETRACKHITPOSITIONSANDCELLID_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

// datamodel
namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}


class CompareTrackHitPositionAndCellId : public GaudiAlgorithm {
public:
  CompareTrackHitPositionAndCellId(const std::string& name, ISvcLocator* svcLoc);
  /**  Initialize.
   *   @return status code
   */
  StatusCode initialize();
  /**  Execute.
   *   @return status code
   */
  StatusCode execute();
  /**  Finalize.
   *   @return status code
   */
  StatusCode finalize();

private:
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;

  /// Handle for tracker hits
  DataHandle<fcc::TrackHitCollection> m_trackHits{"hits/trackerHits", Gaudi::DataHandle::Reader, this};
  /// Handle for tracker hits including position information
  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"hits/positionedTrackerHits",
                                                                      Gaudi::DataHandle::Reader, this};
};

#endif /* TESTGEOMETRY_COMPARETRACKHITPOSITIONSANDCELLID_H */
