//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef TrackClusterAssociationCollection_H
#define  TrackClusterAssociationCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/TrackClusterAssociation.h"
#include "DataObjects/TrackClusterAssociationHandle.h"

typedef std::vector<TrackClusterAssociation> TrackClusterAssociationVector;
typedef std::vector<TrackClusterAssociationHandle> TrackClusterAssociationHandleContainer;

class TrackClusterAssociationCollectionIterator {

  public:
    TrackClusterAssociationCollectionIterator(int index, const TrackClusterAssociationCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const TrackClusterAssociationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const TrackClusterAssociationHandle operator*() const;

    const TrackClusterAssociationCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const TrackClusterAssociationCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class TrackClusterAssociationCollection : public albers::CollectionBase {

public:
  typedef const TrackClusterAssociationCollectionIterator const_iterator;

  TrackClusterAssociationCollection();
//  TrackClusterAssociationCollection(TrackClusterAssociationVector* data, int collectionID);
  ~TrackClusterAssociationCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  TrackClusterAssociationHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const TrackClusterAssociationHandle& get(int index) const;

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
  std::vector<TrackClusterAssociation>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const TrackClusterAssociationHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  TrackClusterAssociationVector* m_data;
  TrackClusterAssociationHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif