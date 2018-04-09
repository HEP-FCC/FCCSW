#ifndef FWCORE_PODIODATASVC_H
#define FWCORE_PODIODATASVC_H

#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IConversionSvc.h"
// PODIO
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include <utility>
// Forward declarations

/** @class PodioEvtSvc EvtDataSvc.h
 *
 *   An EvtDataSvc for PODIO classes
 *
 *  @author B. Hegner
 */
class PodioDataSvc : public DataSvc {
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
  virtual StatusCode registerObject(const std::string& fullPath, DataObject* pObject) final;

  StatusCode readCollection(const std::string& collectionName, int collectionID);

  virtual const CollRegistry& getCollections() const { return m_collections; }
  virtual const CollRegistry& getReadCollections() const { return m_readCollections; }
  virtual podio::CollectionIDTable* getCollectionIDs() { return m_collectionIDs; }

  /// Set the collection IDs (if reading a file)
  void setCollectionIDs(podio::CollectionIDTable* collectionIds);
  /// Resets caches of reader and event store, increases event counter
  void endOfRead();

private:
  /// PODIO reader for ROOT files
  podio::ROOTReader m_reader;
  /// PODIO EventStore, used to initialise collections
  podio::EventStore m_provider;
  /// Counter of the event number
  int m_eventNum{0};
  /// Number of events in the file / to process
  int m_eventMax{-1};

  SmartIF<IConversionSvc> m_cnvSvc;

  // special members for podio handling
  std::vector<std::pair<std::string, podio::CollectionBase*>> m_collections;
  std::vector<std::pair<std::string, podio::CollectionBase*>> m_readCollections;
  podio::CollectionIDTable* m_collectionIDs;

protected:
  /// ROOT file name the input is read from. Set by option filename
  std::string m_filename;
};
#endif  // CORE_PODIODATASVC_H
