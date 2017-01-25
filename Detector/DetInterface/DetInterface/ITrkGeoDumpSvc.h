
#ifndef ITRKGEODUMPSVC_ITRKGEODUMPSVC_H
#define ITRKGEODUMPSVC_ITRKGEODUMPSVC_H

// Gaudi
#include "GaudiKernel/IService.h"

namespace Acts {
  class Surface;
}
class Identifier;


class ITrkGeoDumpSvc: virtual public IService {
public:
  DeclareInterfaceID(ITrkGeoDumpSvc,1,0);

  virtual const Acts::Surface* lookUpTrkSurface(const Identifier id) = 0;
};
#endif /* ITRKGEODUMPSVC_ITRKGEODUMPSVC_H */
