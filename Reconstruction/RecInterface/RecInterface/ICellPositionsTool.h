#ifndef RECINTERFACE_ICELLPOSITIONSTOOL_H
#define RECINTERFACE_ICELLPOSITIONSTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

//DD4hep
#include "DD4hep/Detector.h"

namespace fcc {
class CaloHit;
class CaloHitCollection;
class PositionedCaloHitCollection;
}

/** @class ICellPositionsTool
 *
 *  Abstract interface to FCChh calorimeter cell positions.
 *
 *  @author Coralie Neubueser
 *  @date   2018-01
 */

class ICellPositionsTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ICellPositionsTool, 1, 0);

  virtual void getPositions(const fcc::CaloHitCollection& aCells, fcc::PositionedCaloHitCollection& outputColl) = 0;

  virtual dd4hep::Position xyzPosition(const uint64_t& aCellId) const = 0;
  virtual int layerId(const uint64_t& aCellId) = 0;
};

#endif /* RECINTERFACE_ICELLPOSITIONSTOOL_H */
