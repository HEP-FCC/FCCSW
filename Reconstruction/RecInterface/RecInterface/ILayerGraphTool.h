#ifndef RECINTERFACE_ILAYERGRAPHTOOL
#define RECINTERFACE_ILAYERGRAPHTOOL

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace tricktrack {
class CMGraph;
}

class ILayerGraphTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ILayerGraphTool, 1, 0);

  virtual tricktrack::CMGraph getGraph() = 0;
};

#endif /* RECINTERFACE_ILAYERGRAPHTOOL */
