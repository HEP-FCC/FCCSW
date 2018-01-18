#ifndef RECINTERFACE_ICELLPOSITIONSTOOL_H
#define RECINTERFACE_ICELLPOSITIONSTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

//DD4hep
#include "DD4hep/LCDD.h"

namespace fcc {
class CaloHit;
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

  virtual DD4hep::Geometry::Position getXYZPosition(const fcc::CaloHit& aCell) const = 0;
};

#endif /* RECINTERFACE_ICELLPOSITIONSTOOL_H */
