#ifndef RECINTERFACE_ILAYERGRAPHTOOL
#define RECINTERFACE_ILAYERGRAPHTOOL

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace tricktrack {
class CMGraph;
}

/** \brief Construct the tricktrack graph specifying the seeding layers
 *
 */ 
class ILayerGraphTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ILayerGraphTool, 1, 0);

  /// construct and return the graph describing the seeding layers
  virtual tricktrack::CMGraph graph() = 0;
};

#endif /* RECINTERFACE_ILAYERGRAPHTOOL */
