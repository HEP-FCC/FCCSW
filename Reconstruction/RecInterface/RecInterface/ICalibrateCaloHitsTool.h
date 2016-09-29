#ifndef RECINTERFACE_ICALIBRATECALOHITSTOOL_H
#define RECINTERFACE_ICALIBRATECALOHITSTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"


#include <vector>

/** @class ICalibrateCaloHitsTool
 *
 *  Abstract interface to calorimeter hits calibration tool
 *
 *  @author Jana Faltova
 *  @date   2016-09
 */

class ICalibrateCaloHitsTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ICalibrateCaloHitsTool, 1, 0);

  virtual void calibrate(std::vector<fcc::CaloHit*>& aHits) = 0;
};

#endif /* RECINTERFACE_ICALIBRATECALOHITSTOOL_H */
