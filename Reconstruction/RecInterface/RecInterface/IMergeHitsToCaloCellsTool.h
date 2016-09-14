#ifndef RECINTERFACE_IMERGEHITSTOCALOCELLSTOOL_H
#define RECINTERFACE_IMERGEHITSTOCALOCELLSTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

#include <vector>
/** @class IMergeHitsToCaloCellsTool
 *
 *  Abstract interface to merging hits to cell tool
 *
 *  @author Jana Faltova
 *  @date   2016-08
 */

class IMergeHitsToCaloCellsTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IMergeHitsToCaloCellsTool, 1, 0);
  
  virtual void DoMerge(const fcc::CaloHitCollection& aHits, std::vector<fcc::CaloHit*>& aCells) = 0;
};

#endif /* RECINTERFACE_IMERGEHITSTOCALOCELLSTOOL_H */
