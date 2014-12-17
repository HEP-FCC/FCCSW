//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef CaloClusterCollection_H
#define  CaloClusterCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/CaloCluster.h"
#include "DataObjects/CaloClusterHandle.h"

typedef std::vector<CaloCluster> CaloClusterVector;
typedef std::vector<CaloClusterHandle> CaloClusterHandleContainer;

class CaloClusterCollectionIterator {

  public:
    CaloClusterCollectionIterator(int index, const CaloClusterCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const CaloClusterCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const CaloClusterHandle operator*() const;

    const CaloClusterCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const CaloClusterCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class CaloClusterCollection : public albers::CollectionBase {

public:
  typedef const CaloClusterCollectionIterator const_iterator;

  CaloClusterCollection();
//  CaloClusterCollection(CaloClusterVector* data, int collectionID);
  ~CaloClusterCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  CaloClusterHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const CaloClusterHandle& get(int index) const;

  /// Currently does nothing
  void prepareForWrite(const albers::Registry* registry);
  void prepareAfterRead(albers::Registry* registry);
  void setPODsAddress(const void* address);

  void setID(unsigned ID){m_collectionID = ID;};

  // support for the iterator protocol
  const const_iterator begin() const {
    return const_iterator(0, this);
  }
  const	const_iterator end() const {
    return const_iterator(m_handles.size(), this);
  }

//  std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections();

  /// returns the address of the pointer to the data buffer
  void* _getRawBuffer() { return (void*)&m_data;};

  /// returns the pointer to the data buffer
  std::vector<CaloCluster>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const CaloClusterHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  CaloClusterVector* m_data;
  CaloClusterHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif