//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef EventInfoCollection_H
#define  EventInfoCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/EventInfo.h"
#include "DataObjects/EventInfoHandle.h"

typedef std::vector<EventInfo> EventInfoVector;
typedef std::vector<EventInfoHandle> EventInfoHandleContainer;

class EventInfoCollectionIterator {

  public:
    EventInfoCollectionIterator(int index, const EventInfoCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const EventInfoCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const EventInfoHandle operator*() const;

    const EventInfoCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const EventInfoCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class EventInfoCollection : public albers::CollectionBase {

public:
  typedef const EventInfoCollectionIterator const_iterator;

  EventInfoCollection();
//  EventInfoCollection(EventInfoVector* data, int collectionID);
  ~EventInfoCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  EventInfoHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const EventInfoHandle& get(int index) const;

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
  std::vector<EventInfo>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const EventInfoHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  EventInfoVector* m_data;
  EventInfoHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif