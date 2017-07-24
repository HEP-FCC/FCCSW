#ifndef IPAPASCALORIMETERSVC_H
#define IPAPASCALORIMETERSVC_H

#include "GaudiKernel/IService.h"

namespace papas {
  class Calorimeter;
}

/** @author: Alice Robson
 *  Service for the papas detector *
 */
class IPapasCalorimeterSvc : virtual public IService {
public:
  // InterfaceID
  DeclareInterfaceID(IPapasCalorimeterSvc, 1, 0);
  // receive Reconstruction geometry
  virtual std::shared_ptr<papas::Calorimeter> calorimeter() const = 0;
};

#endif  // IPAPASCALORIMETERSVC_H
