#ifndef RECINTERFACE_ICALIBRATECALOHITSTOOL_H
#define RECINTERFACE_ICALIBRATECALOHITSTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

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

  virtual void calibrate(std::unordered_map<uint64_t, double>& aHits) = 0;
};

#endif /* RECINTERFACE_ICALIBRATECALOHITSTOOL_H */
