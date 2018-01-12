#ifndef RECINTERFACE_ICELLPOSITIONSTOOL_H
#define RECINTERFACE_ICELLPOSITIONSTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace fcc {
class CaloHitCollection;
class PositionedCaloHitCollection;
}

/** @class ICellPositionsTool
 *
 *
 *  @author Coralie Neubueser
 *  @date   2018-01
 */

class ICellPositionsTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ICellPositionsTool, 1, 0);

  virtual void getPositions(const fcc::CaloHitCollection& aCells, fcc::PositionedCaloHitCollection& outputColl) = 0;
};

#endif /* RECINTERFACE_ICELLPOSITIONSTOOL_H */
