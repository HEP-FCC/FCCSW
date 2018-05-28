#ifndef DIGITIZATION_ICLUSTERWRITERTOOL_H
#define DIGITIZATION_ICLUSTERWRITERTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace sim {
class FCCPlanarCluster;
}

/** @class
 */

class IClusterWriter : virtual public IAlgTool {
public:
  DeclareInterfaceID(IClusterWriter, 1, 0);

  /**
   */
  virtual StatusCode write(const sim::FCCPlanarCluster& cluster, int eventNr = 0) = 0;
};
#endif /* DIGITIZATION_ICLUSTERWRITERTOOL_H */
