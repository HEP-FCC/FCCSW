#ifndef IDUMMYTRKGEOSVC_H
#define IDUMMYTRKGEOSVC_H

#include "GaudiKernel/IService.h"

namespace Acts {
    class TrackingGeometry;
}

class IDummyTrkGeoSvc: virtual public IService {
public:
  DeclareInterfaceID(IDummyTrkGeoSvc,1,0);
  virtual std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry() const = 0;
  virtual ~IDummyTrkGeoSvc() {}
};

#endif //IDUMMYTRKGEOSVC_H
