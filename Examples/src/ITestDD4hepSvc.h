#ifndef TESTDD4HEP_ITESTDD4HEPSVC_H
#define TESTDD4HEP_ITESTDD4HEPSVC_H

// Gaudi
#include "GaudiKernel/IService.h"

/** @class ITestDD4hepSvc Test/TestDD4hep/TestDD4hep/ITestDD4hepSvc.h ITestDD4hepSvc.h
 *
 *  Interface to the test service.
 *
 *  @author Anna Zaborowska
 */

class ITestDD4hepSvc : virtual public IService {
public:
  DeclareInterfaceID(ITestDD4hepSvc, 1, 0);
};
#endif /* TESTDD4HEP_ITESTDD4HEPSVC_H */
