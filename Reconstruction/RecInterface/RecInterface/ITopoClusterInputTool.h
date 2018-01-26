#ifndef RECINTERFACE_ITOPOCLUSTERINPUTTOOL_H
#define RECINTERFACE_ITOPOCLUSTERINPUTTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace fcc {
class CaloHit;
}
/** @class ITopoClusterInputTool RecInterface/RecInterface/ITopoClusterInput.h ITopoClusterInputTool.h
 *
 *  Abstract interface to topo cluster input tool.
 *
 *  @author Coralie Neubueser
 */

class ITopoClusterInputTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ITopoClusterInputTool, 1, 0);

  virtual std::map<uint64_t, double> cellIdMap() = 0;

  virtual fcc::CaloHit cellByCellId(uint64_t cellId) = 0;
 };

#endif /* RECINTERFACE_ITOPOCLUSTERINPUTTOOL_H */
