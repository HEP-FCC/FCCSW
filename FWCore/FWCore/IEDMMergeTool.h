#ifndef FWCORE_IEDMMERGETOOL_H
#define FWCORE_IEDMMERGETOOL_H

#include "GaudiKernel/IAlgTool.h"

namespace podio {
class EventStore;
}

class IEDMMergeTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IEDMMergeTool, 1, 0);
  /// read pileup collection from store and handle them internally
  virtual StatusCode readPileupCollection(podio::EventStore& store) = 0;
  /// merge internally stored pileup (and signal) collections and make them accessible (typically via event store)
  virtual StatusCode mergeCollections() = 0;

  /// read signal collection from event store and
  virtual StatusCode readSignal() = 0;
};

#endif  // FWCORE_IEDMMERGETOOL_H
