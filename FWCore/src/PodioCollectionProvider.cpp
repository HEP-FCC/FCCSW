#include "FWCore/PodioCollectionProvider.h"
#include "FWCore/PodioDataSvc.h"
#include "FWCore/DataWrapper.h"

#include "TROOT.h"
#include "TTree.h"


namespace fwcore {  
bool PodioCollectionProvider::get(int collectionID, podio::CollectionBase*& collection) const {
  auto result = std::find_if(begin(m_collections), end(m_collections),
                             [collectionID](const std::pair<int, podio::CollectionBase*>& item)->bool { return collectionID==item.first; }
  );
  if (result != end(m_collections)) {
    collection = result->second;
    return true;
  }
  collection = nullptr;
  return false;
}

bool PodioCollectionProvider::addCollection(int id, podio::CollectionBase* coll) {
  m_collections[id] = coll;
  return true;
}

void PodioCollectionProvider::clear() {
  m_collections.clear();
}

void PodioCollectionProvider::setReferences() {
  for (auto it = m_collections.begin(), end = m_collections.end(); it != end; ++it) {
    if (it->second->referenceCollections()->size() > 0) {
      it->second->setReferences(this);
    }
  }
}

}
