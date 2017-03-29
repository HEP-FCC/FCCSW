#ifndef SIMG4INTERFACE_ISIMG4REGIONTOOL_H
#define SIMG4INTERFACE_ISIMG4REGIONTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class ISimG4RegionTool SimG4Interface/SimG4Interface/ISimG4RegionTool.h ISimG4RegionTool.h
 *
 *  Interface to the tool creating region.
 *
 *  @author Anna Zaborowska
 */

class ISimG4RegionTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ISimG4RegionTool, 1, 0);

  /**  Create region.
   *   @return status code
   */
  virtual StatusCode create() = 0;
};
#endif /* SIMG4INTERFACE_ISIMG4REGIONTOOL_H */
