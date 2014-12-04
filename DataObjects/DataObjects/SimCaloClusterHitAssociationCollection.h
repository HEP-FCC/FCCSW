//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef SimCaloClusterHitAssociationCollection_H
#define  SimCaloClusterHitAssociationCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/SimCaloClusterHitAssociation.h"
#include "DataObjects/SimCaloClusterHitAssociationHandle.h"

typedef std::vector<SimCaloClusterHitAssociation> SimCaloClusterHitAssociationVector;
typedef std::vector<SimCaloClusterHitAssociationHandle> SimCaloClusterHitAssociationHandleContainer;

class SimCaloClusterHitAssociationCollectionIterator {

  public:
    SimCaloClusterHitAssociationCollectionIterator(int index, const SimCaloClusterHitAssociationCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const SimCaloClusterHitAssociationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const SimCaloClusterHitAssociationHandle operator*() const;

    const SimCaloClusterHitAssociationCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const SimCaloClusterHitAssociationCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class SimCaloClusterHitAssociationCollection : public albers::CollectionBase {

public:
  typedef const SimCaloClusterHitAssociationCollectionIterator const_iterator;

  SimCaloClusterHitAssociationCollection();
//  SimCaloClusterHitAssociationCollection(SimCaloClusterHitAssociationVector* data, int collectionID);
  ~SimCaloClusterHitAssociationCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  SimCaloClusterHitAssociationHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const SimCaloClusterHitAssociationHandle& get(int index) const;

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
  std::vector<SimCaloClusterHitAssociation>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const SimCaloClusterHitAssociationHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  SimCaloClusterHitAssociationVector* m_data;
  SimCaloClusterHitAssociationHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif