//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef VertexCollection_H
#define  VertexCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/Vertex.h"
#include "DataObjects/VertexHandle.h"

typedef std::vector<Vertex> VertexVector;
typedef std::vector<VertexHandle> VertexHandleContainer;

class VertexCollectionIterator {

  public:
    VertexCollectionIterator(int index, const VertexCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const VertexCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const VertexHandle operator*() const;

    const VertexCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const VertexCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class VertexCollection : public albers::CollectionBase {

public:
  typedef const VertexCollectionIterator const_iterator;

  VertexCollection();
//  VertexCollection(VertexVector* data, int collectionID);
  ~VertexCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  VertexHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const VertexHandle& get(int index) const;

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
  std::vector<Vertex>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const VertexHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  VertexVector* m_data;
  VertexHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif