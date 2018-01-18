#ifndef IPAPASDETSVC_H
#define IPAPASDETSVC_H

#include "GaudiKernel/IService.h"

namespace papas {
class Detector;
}

/** @author: Alice Robson
 *  Service for the papas detector *
 */
class IPapasDetSvc : virtual public IService {
public:
  // InterfaceID
  DeclareInterfaceID(IPapasDetSvc, 1, 0);
  // return the detector service
  virtual std::shared_ptr<papas::Detector> detector() const = 0;
};

#endif  // IPAPASDETSVC_H
