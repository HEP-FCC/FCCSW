#ifndef RECINTERFACE_IMERGECALOHITSTOOL_H
#define RECINTERFACE_IMERGECALOHITSTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

#include <vector>
/** @class IMergeCaloHitsTool
 *
 *  Abstract interface for merging CaloHits
 *
 *  @author Jana Faltova
 *  @date   2016-09
 */

class IMergeCaloHitsTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IMergeCaloHitsTool, 1, 0);

  virtual std::vector<fcc::CaloHit*> mergeOneCollection(const fcc::CaloHitCollection& aHits) = 0;
  virtual std::vector<fcc::CaloHit*> mergeTwoVectors( const std::vector<fcc::CaloHit*>& aHits1, const std::vector<fcc::CaloHit*>& aHits2) = 0;
 
};

#endif /* RECINTERFACE_IMERGEHITSTOCALOCELLSTOOL_H */
