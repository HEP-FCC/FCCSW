#ifndef FWCORE_PODIODATASVC_H
#define FWCORE_PODIODATASVC_H

#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"
#include <utility>
// Forward declarations


/** @class PodioEvtSvc EvtDataSvc.h
 *
 *   An EvtDataSvc for PODIO classes
 *
 *  @author B. Hegner
 */
class PodioDataSvc  : public DataSvc   {
public:

  typedef std::vector<std::pair<std::string, podio::CollectionBase*>> CollRegistry;

  virtual StatusCode initialize();
  virtual StatusCode reinitialize();
  virtual StatusCode finalize();
  virtual StatusCode clearStore();

  /// Standard Constructor
  PodioDataSvc(const std::string& name, ISvcLocator* svc);

  /// Standard Destructor
  virtual ~PodioDataSvc();

  // Use DataSvc functionality except where we override
  using DataSvc::registerObject;
  /// Overriding standard behaviour of evt service
  /// Register object with the data store.
  virtual StatusCode registerObject(  const std::string& fullPath, DataObject* pObject ) final;

  virtual const CollRegistry& getCollections() const {return m_collections;}
  virtual podio::CollectionIDTable* getCollectionIDs() {return m_collectionIDs;}

  /// Set the collection IDs (if reading a file)
  void setCollectionIDs(podio::CollectionIDTable* collectionIds);

private:
  SmartIF<IConversionSvc> m_cnvSvc;

  // special members for podio handling
  std::vector<std::pair<std::string, podio::CollectionBase*>> m_collections;
  podio::CollectionIDTable* m_collectionIDs;
};
#endif // CORE_PODIODATASVC_H
