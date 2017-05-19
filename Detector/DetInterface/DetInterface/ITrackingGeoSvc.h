#ifndef ITRACKINGGEOSVC_H
#define ITRACKINGGEOSVC_H

#include "GaudiKernel/IService.h"

namespace Acts {
class TrackingGeometry;
}

/// @author: Julia Hrdinka
class ITrackingGeoSvc : virtual public IService {
public:
  // InterfaceID
  DeclareInterfaceID(ITrackingGeoSvc, 1, 0);
  // receive Reconstruction geometry
  virtual std::shared_ptr<Acts::TrackingGeometry> trackingGeometry() const = 0;
  virtual ~ITrackingGeoSvc() {}
};

#endif  // ITRACKINGGEOSVC_H
