#ifndef FWCORE_ALBERSDATASVC_H
#define FWCORE_ALBERSDATASVC_H

#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "albers/Registry.h"
#include "albers/CollectionBase.h"
#include <utility>
// Forward declarations


/** @class AlbersEvtSvc EvtDataSvc.h
 *
 *   An EvtDataSvc for Albers classes
 *
 *  @author B. Hegner
 */
class AlbersDataSvc  : public DataSvc   {
public:

  typedef std::vector<std::pair<std::string, albers::CollectionBase*>> CollRegistry;

  virtual StatusCode initialize();
  virtual StatusCode reinitialize();
  virtual StatusCode finalize();
  virtual StatusCode clearStore();

  /// Standard Constructor
  AlbersDataSvc(const std::string& name, ISvcLocator* svc);

  /// Standard Destructor
  virtual ~AlbersDataSvc();

  /// Overriding standard behaviour of evt service

  /// Register object with the data store.
  virtual StatusCode registerObject(  const std::string& fullPath, DataObject* pObject );

  virtual const CollRegistry& getCollections() const {return m_collections;};
  virtual albers::Registry* getRegistry() {return &m_registry;};

private:
  SmartIF<IConversionSvc> m_cnvSvc;

  // special members for albers handling
  albers::Registry   m_registry;  
  std::vector<std::pair<std::string, albers::CollectionBase*>> m_collections;

};
#endif // CORE_ALBERSDATASVC_H
