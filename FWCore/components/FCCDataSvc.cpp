#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IConversionSvc.h"

#include "FCCDataSvc.h"

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_SERVICE_FACTORY(FCCDataSvc)

/// Standard Constructor
FCCDataSvc::FCCDataSvc(const std::string& name,ISvcLocator* svc):
  AlbersDataSvc(name,svc) {
}

/// Standard Destructor
FCCDataSvc::~FCCDataSvc() {
}
