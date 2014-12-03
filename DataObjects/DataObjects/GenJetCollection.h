//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef GenJetCollection_H
#define  GenJetCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/GenJet.h"
#include "DataObjects/GenJetHandle.h"

typedef std::vector<GenJet> GenJetVector;
typedef std::vector<GenJetHandle> GenJetHandleContainer;

class GenJetCollectionIterator {

  public:
    GenJetCollectionIterator(int index, const GenJetCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const GenJetCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const GenJetHandle operator*() const;

    const GenJetCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const GenJetCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class GenJetCollection : public albers::CollectionBase {

public:
  typedef const GenJetCollectionIterator const_iterator;

  GenJetCollection();
//  GenJetCollection(GenJetVector* data, int collectionID);
  ~GenJetCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  GenJetHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const GenJetHandle& get(int index) const;

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
  std::vector<GenJet>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const GenJetHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  GenJetVector* m_data;
  GenJetHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif