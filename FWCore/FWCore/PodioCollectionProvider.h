#ifndef FWCORE_PODIOCOLLECTIONPROVIDER_H
#define FWCORE_PODIOCOLLECTIONPROVIDER_H

// PODIO
#include "podio/ICollectionProvider.h"
#include "podio/CollectionBase.h"
// STL
#include <map>

/** @class fwcore::PodioCollectionProvider FWCore/FWCore/PodioCollectionProvider.h PodioCollectionProvider.h
 *
 *  Class that stores map from ID to collections (IDs set by PodioDataSvc)
 *  This is just a helper class to avoid double inheritance
 *
 *  @author J. Lingemann
 */
namespace fwcore {
class PodioCollectionProvider: virtual podio::ICollectionProvider {
public:
  /// Default constructor
  PodioCollectionProvider() {};

  /// Destructor
  virtual ~PodioCollectionProvider() {};

  virtual bool get(int collectionID, podio::CollectionBase*& collection) const final;
  bool addCollection(int id, podio::CollectionBase* coll);
  void clear();
  void setReferences();

private:
  /// Map from collection ID to collection pointer.
  /// Note that these are only observing pointers (ownership is with the PodioDataSvc)
  std::map<int, podio::CollectionBase*> m_collections;
};
}

#endif /* define FWCORE_COLLECTIONPROVIDER_H */
