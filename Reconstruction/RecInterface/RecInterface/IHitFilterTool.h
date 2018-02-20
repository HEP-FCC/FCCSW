#ifndef RECINTERFACE_IHITFILTERTOOL_H
#define RECINTERFACE_IHITFILTERTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"



namespace fcc {
class TrackHit;
}


class IHitFilterTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IHitFilterTool, 1, 0);

  virtual void setIds(unsigned int systemId, unsigned int layerId) = 0;
  virtual bool filter(fcc::TrackHit hit) = 0;
};

#endif /* RECINTERFACE_IHITFILTERTOOL_H */
