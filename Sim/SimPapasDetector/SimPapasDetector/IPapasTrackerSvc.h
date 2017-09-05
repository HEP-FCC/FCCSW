#ifndef IPAPASTRACKERSVC_H
#define IPAPASTRACKERSVC_H

#include "GaudiKernel/IService.h"

namespace papas {
class Tracker;
}

/** @author: Alice Robson
 *  Service for the papas tracker *
 */
class IPapasTrackerSvc : virtual public IService {
public:
  DeclareInterfaceID(IPapasTrackerSvc, 1, 0);
  /// Return the papas tracker service
  virtual std::shared_ptr<papas::Tracker> tracker() const = 0;
};

#endif  // IPAPASTRACKERSVC_H
