
#ifndef RECINTERFACE_ITRKVOLUMEMANAGERSVC_H
#define RECINTERFACE_ITRKVOLUMEMANAGERSVC_H

// Gaudi
#include "GaudiKernel/IService.h"

namespace Acts {
  class Surface;
}
class Identifier;


class ITrkVolumeManagerSvc: virtual public IService {
public:
  DeclareInterfaceID(ITrkVolumeManagerSvc,1,0);

  virtual const Acts::Surface* lookUpTrkSurface(const Identifier id) = 0;
};
#endif /* RECINTERFACE_ITRKVOLUMEMANAGERSVC_H */
