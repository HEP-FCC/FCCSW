#ifndef RECINTERFACE_INOISECALOCELLSTOOL_H
#define RECINTERFACE_INOISECALOCELLSTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class INoiseCaloCellsTool
 *
 *  Abstract interface to calorimeter noise tool
 *
 *  @author Jana Faltova
 *  @date   2016-09
 */

class INoiseCaloCellsTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(INoiseCaloCellsTool, 1, 0);

  virtual void addRandomCellNoise(std::unordered_map<uint64_t, double>& aCells) = 0;
  virtual void filterCellNoise(std::unordered_map<uint64_t, double>& aCells) = 0;
};

#endif /* RECINTERFACE_INOISECALOCELLSTOOL_H */
