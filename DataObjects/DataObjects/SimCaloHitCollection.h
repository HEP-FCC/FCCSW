//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef SimCaloHitCollection_H
#define  SimCaloHitCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/SimCaloHit.h"
#include "DataObjects/SimCaloHitHandle.h"

typedef std::vector<SimCaloHit> SimCaloHitVector;
typedef std::vector<SimCaloHitHandle> SimCaloHitHandleContainer;

class SimCaloHitCollectionIterator {

  public:
    SimCaloHitCollectionIterator(int index, const SimCaloHitCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const SimCaloHitCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const SimCaloHitHandle operator*() const;

    const SimCaloHitCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const SimCaloHitCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class SimCaloHitCollection : public albers::CollectionBase {

public:
  typedef const SimCaloHitCollectionIterator const_iterator;

  SimCaloHitCollection();
//  SimCaloHitCollection(SimCaloHitVector* data, int collectionID);
  ~SimCaloHitCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  SimCaloHitHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const SimCaloHitHandle& get(int index) const;

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
  std::vector<SimCaloHit>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const SimCaloHitHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  SimCaloHitVector* m_data;
  SimCaloHitHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif