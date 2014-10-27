#ifndef FWCORE_FCCDATASVC_H
#define FWCORE_FCCDATASVC_H

#include "FWCore/AlbersDataSvc.h"

class FCCDataSvc  : public AlbersDataSvc   {
  friend class SvcFactory<FCCDataSvc>;

 public:
  /// Standard Constructor
  FCCDataSvc(const std::string& name, ISvcLocator* svc);

  /// Standard Destructor
  virtual ~FCCDataSvc();

};
#endif // FWCORE_FCCDATASVC_H
