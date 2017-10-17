#ifndef IPAPASFIELDSVC_H
#define IPAPASFIELDSVC_H

#include "GaudiKernel/IService.h"

namespace papas {
class Field;
}

/** @author: Alice Robson
 *  Service for the papas field *
 */
class IPapasFieldSvc : virtual public IService {
public:
  // InterfaceID
  DeclareInterfaceID(IPapasFieldSvc, 1, 0);
  // return the papas field
  virtual std::shared_ptr<papas::Field> field() const = 0;
};

#endif  // IPAPASFIELDSVC_H
