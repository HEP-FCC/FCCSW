#ifndef RECINTERFACE_INOISECONSTTOOL_H
#define RECINTERFACE_INOISECONSTTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class INoiseConstTool
 *
 *  Abstract interface to get calorimeter noise per cell tool
 *  @author Coralie Neubueser
 *  @date   2018-01
 */

class INoiseConstTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(INoiseConstTool, 1, 0);

  virtual double getNoiseConstantPerCell(uint64_t aCellID) = 0;
};

#endif /* RECINTERFACE_INOISECONSTTOOL_H */
