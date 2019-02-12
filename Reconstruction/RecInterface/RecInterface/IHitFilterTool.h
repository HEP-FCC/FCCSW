#ifndef RECINTERFACE_IHITFILTERTOOL_H
#define RECINTERFACE_IHITFILTERTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace fcc {
class TrackHit;
}

/** @class IHitFilterTool
 *  Stateful filter of FCC hits based on their CellId.
 *
 *  Used in track reconstruction to get hits on the same layer, but
 *  is the interface to be used whenever a filter is applied to fcc hits.
 */
class IHitFilterTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IHitFilterTool, 1, 0);

  // set system and layer ids, for the special case of filtering based on layers
  virtual void setIds(unsigned int systemId, unsigned int layerId) = 0;
  // more general way to set filter state
  virtual void setState(std::vector<double> params) = 0;
  // decide whether or not to filter this hit, based on the state of this class
  virtual bool filter(fcc::TrackHit hit) = 0;
};

#endif /* RECINTERFACE_IHITFILTERTOOL_H */
