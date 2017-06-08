#ifndef ITRACKINGGEOSVC_H
#define ITRACKINGGEOSVC_H

#include "GaudiKernel/IService.h"

namespace Acts {
class TrackingGeometry;
}

/** @author: Julia Hrdinka
 *  Service for the tracking geometry ( concrete implementations
 *  may be using the DD4hepPlugin or constructing it explicitly )
 *  Returns the top level geometry object.
 *
 */
class ITrackingGeoSvc : virtual public IService {
public:
  // InterfaceID
  DeclareInterfaceID(ITrackingGeoSvc, 1, 0);
  // receive Reconstruction geometry
  virtual std::shared_ptr<Acts::TrackingGeometry> trackingGeometry() const = 0;
};

#endif  // ITRACKINGGEOSVC_H
