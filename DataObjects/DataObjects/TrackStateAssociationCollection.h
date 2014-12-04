//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef TrackStateAssociationCollection_H
#define  TrackStateAssociationCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/TrackStateAssociation.h"
#include "DataObjects/TrackStateAssociationHandle.h"

typedef std::vector<TrackStateAssociation> TrackStateAssociationVector;
typedef std::vector<TrackStateAssociationHandle> TrackStateAssociationHandleContainer;

class TrackStateAssociationCollectionIterator {

  public:
    TrackStateAssociationCollectionIterator(int index, const TrackStateAssociationCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const TrackStateAssociationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const TrackStateAssociationHandle operator*() const;

    const TrackStateAssociationCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const TrackStateAssociationCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class TrackStateAssociationCollection : public albers::CollectionBase {

public:
  typedef const TrackStateAssociationCollectionIterator const_iterator;

  TrackStateAssociationCollection();
//  TrackStateAssociationCollection(TrackStateAssociationVector* data, int collectionID);
  ~TrackStateAssociationCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  TrackStateAssociationHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const TrackStateAssociationHandle& get(int index) const;

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
  std::vector<TrackStateAssociation>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const TrackStateAssociationHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  TrackStateAssociationVector* m_data;
  TrackStateAssociationHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif