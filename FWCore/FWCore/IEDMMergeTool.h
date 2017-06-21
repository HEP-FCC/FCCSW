#ifndef FWCORE_IEDMMERGETOOL_H
#define FWCORE_IEDMMERGETOOL_H

#include "GaudiKernel/IAlgTool.h"

namespace podio {
  class EventStore;
}

class IEDMMergeTool: virtual public IAlgTool {
public: 
  DeclareInterfaceID(IEDMMergeTool, 1, 0);

  virtual StatusCode readPileupCollection(podio::EventStore& store) = 0;

  virtual StatusCode mergeCollections() = 0;

};


#endif // FWCORE_IEDMMERGETOOL_H
