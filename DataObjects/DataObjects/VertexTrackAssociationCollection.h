//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef VertexTrackAssociationCollection_H
#define  VertexTrackAssociationCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/VertexTrackAssociation.h"
#include "DataObjects/VertexTrackAssociationHandle.h"

typedef std::vector<VertexTrackAssociation> VertexTrackAssociationVector;
typedef std::vector<VertexTrackAssociationHandle> VertexTrackAssociationHandleContainer;

class VertexTrackAssociationCollectionIterator {

  public:
    VertexTrackAssociationCollectionIterator(int index, const VertexTrackAssociationCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const VertexTrackAssociationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const VertexTrackAssociationHandle operator*() const;

    const VertexTrackAssociationCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const VertexTrackAssociationCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class VertexTrackAssociationCollection : public albers::CollectionBase {

public:
  typedef const VertexTrackAssociationCollectionIterator const_iterator;

  VertexTrackAssociationCollection();
//  VertexTrackAssociationCollection(VertexTrackAssociationVector* data, int collectionID);
  ~VertexTrackAssociationCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  VertexTrackAssociationHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const VertexTrackAssociationHandle& get(int index) const;

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
  std::vector<VertexTrackAssociation>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const VertexTrackAssociationHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  VertexTrackAssociationVector* m_data;
  VertexTrackAssociationHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif