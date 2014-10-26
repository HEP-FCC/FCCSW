#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IConversionSvc.h"

#include "FWCore/AlbersDataSvc.h"

/// Service initialisation
StatusCode AlbersDataSvc::initialize()    {
  // Nothing to do: just call base class initialisation
  StatusCode      status  = DataSvc::initialize();
  ISvcLocator*    svc_loc = serviceLocator();

  // Attach data loader facility
  m_cnvSvc = svc_loc->service("EventPersistencySvc");
  status = setDataLoader( m_cnvSvc );
  return status;
}
/// Service reinitialisation
StatusCode AlbersDataSvc::reinitialize()    {
  // Do nothing for this service
  return StatusCode::SUCCESS;
}
/// Service finalization
StatusCode AlbersDataSvc::finalize()    {
  m_cnvSvc = 0; // release
  DataSvc::finalize().ignore();
  return StatusCode::SUCCESS ;
}

StatusCode AlbersDataSvc::clearStore()    {
  DataSvc::clearStore().ignore();
  m_collections.clear();
  return StatusCode::SUCCESS ;
}

/// Standard Constructor
AlbersDataSvc::AlbersDataSvc(const std::string& name,ISvcLocator* svc):
  DataSvc(name,svc) {
}

/// Standard Destructor
AlbersDataSvc::~AlbersDataSvc() {
}

StatusCode AlbersDataSvc::registerObject(  const std::string& fullPath, DataObject* pObject ) {
  albers::CollectionBase* coll = dynamic_cast<albers::CollectionBase*>(pObject);
  if (coll!=0){
    m_registry.registerPOD(coll, fullPath);
    m_collections.emplace_back(std::make_pair(fullPath,coll)); 
  }
  return DataSvc::registerObject(fullPath,pObject);
}
