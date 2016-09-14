#ifndef RECINTERFACE_IPREPAREEMPTYCALOCELLSTOOL_H
#define RECINTERFACE_IPREPAREEMPTYCALOCELLSTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

#include <vector>

/** @class IPrepareEmptyCaloCellsTool
 *
 *  Abstract interface to calorimeter noise tool
 *
 *  @author Jana Faltova
 *  @date   2016-08
 */

class IPrepareEmptyCaloCellsTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IPrepareEmptyCaloCellsTool, 1, 0);

  //virtual fcc::CaloHitCollection* PrepareEmptyCells() = 0;
  virtual std::vector<fcc::CaloHit*> PrepareEmptyCells() = 0;

};

#endif /* RECINTERFACE_IPREPAREEMPTYCALOCELLSTOOL_H */
