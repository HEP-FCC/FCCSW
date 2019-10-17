#ifndef FWCORE_FCCDATASVC_H
#define FWCORE_FCCDATASVC_H

#include "FWCore/PodioDataSvc.h"

class FCCDataSvc : public PodioDataSvc {

public:
  /// Standard Constructor
  FCCDataSvc(const std::string& name, ISvcLocator* svc);

  /// Standard Destructor
  virtual ~FCCDataSvc();
};
#endif  // FWCORE_FCCDATASVC_H
