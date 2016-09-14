#ifndef RECINTERFACE_INOISECALOCELLSTOOL_H
#define RECINTERFACE_INOISECALOCELLSTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

#include <vector>

/** @class INoiseCaloCellsTool
 *
 *  Abstract interface to calorimeter noise tool
 *
 *  @author Jana Faltova
 *  @date   2016-08
 */

class INoiseCaloCellsTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(INoiseCaloCellsTool, 1, 0);

  virtual void AddNoise(std::vector<fcc::CaloHit*>& aCells) = 0;
};

#endif /* RECINTERFACE_INOISECALOCELLSTOOL_H */
