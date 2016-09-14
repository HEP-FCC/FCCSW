#ifndef RECINTERFACE_ICALIBRATECALOCELLSTOOL_H
#define RECINTERFACE_ICALIBRATECALOCELLSTOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"


#include <vector>

/** @class ICalibrateCaloCellsTool
 *
 *  Abstract interface to calorimeter cells calibration tool
 *
 *  @author Jana Faltova
 *  @date   2016-08
 */

class ICalibrateCaloCellsTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ICalibrateCaloCellsTool, 1, 0);

  virtual void Calibrate(std::vector<fcc::CaloHit*>& aCells) = 0;
};

#endif /* RECINTERFACE_ICALIBRATECALOCELLSTOOL_H */
