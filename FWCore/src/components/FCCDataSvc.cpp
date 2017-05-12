#include "FCCDataSvc.h"

#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SvcFactory.h"

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_SERVICE_FACTORY(FCCDataSvc)

/// Standard Constructor
FCCDataSvc::FCCDataSvc(const std::string& name, ISvcLocator* svc) : PodioDataSvc(name, svc) {
  declareProperty("input", m_filename = "", "Name of the file to read");
}

/// Standard Destructor
FCCDataSvc::~FCCDataSvc() {}
