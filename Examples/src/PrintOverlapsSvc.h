#ifndef EXAMPLES_PRINTOVERLAPSVC_H
#define EXAMPLES_PRINTOVERLAPSVC_H

#include "ITestDD4hepSvc.h"

// Gaudi
#include "GaudiKernel/Service.h"

class PrintOverlapsSvc : public extends1<Service, ITestDD4hepSvc> {
public:
  /// Constructor.
  explicit PrintOverlapsSvc(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Finalize.
  virtual StatusCode finalize();
  /// Destructor
  virtual ~PrintOverlapsSvc() {}
};

#endif /* EXAMPLES_PRINTOVERLAPSVC_H */
