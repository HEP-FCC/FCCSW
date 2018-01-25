#ifndef RECINTERFACE_ICALOCREATEMAP_H
#define RECINTERFACE_ICALOCREATEMAP_H

// Gaudi
#include "GaudiKernel/IService.h"

/** @class ICaloCreateMap RecInterface/RecInterface/ICaloCreateMap.h ICaloCreateMap.h
 *
 *  Interface to the service creating a map for the calorimetry.
 *
 *  @author Anna Zaborowska
 */

class ICaloCreateMap : virtual public IService {
public:
  DeclareInterfaceID(ICaloCreateMap, 1, 0);
};
#endif /* RECINTERFACE_ICALOCREATEMAP_H */
