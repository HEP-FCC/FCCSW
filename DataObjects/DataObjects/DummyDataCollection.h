//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef DummyDataCollection_H
#define  DummyDataCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/DummyData.h"
#include "DataObjects/DummyDataHandle.h"

typedef std::vector<DummyData> DummyDataVector;
typedef std::deque<DummyDataHandle> DummyDataHandleContainer;

class DummyDataCollectionIterator {

  public:
    DummyDataCollectionIterator(int index, const DummyDataCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const DummyDataCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const DummyDataHandle operator*() const;

    const DummyDataCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const DummyDataCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class DummyDataCollection : public albers::CollectionBase {

public:
  typedef const DummyDataCollectionIterator const_iterator;

  DummyDataCollection();
//  DummyDataCollection(DummyDataVector* data, int collectionID);
  ~DummyDataCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  DummyDataHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const DummyDataHandle& get(int index) const;

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

  void* _getRawBuffer(){ return (void*)&m_data;};
  std::vector<DummyData>* _getBuffer(){ return m_data;};
private:
  unsigned m_collectionID;
  DummyDataVector* m_data;
  DummyDataHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif