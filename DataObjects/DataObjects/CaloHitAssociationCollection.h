//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef CaloHitAssociationCollection_H
#define  CaloHitAssociationCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/CaloHitAssociation.h"
#include "DataObjects/CaloHitAssociationHandle.h"

typedef std::vector<CaloHitAssociation> CaloHitAssociationVector;
typedef std::vector<CaloHitAssociationHandle> CaloHitAssociationHandleContainer;

class CaloHitAssociationCollectionIterator {

  public:
    CaloHitAssociationCollectionIterator(int index, const CaloHitAssociationCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const CaloHitAssociationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const CaloHitAssociationHandle operator*() const;

    const CaloHitAssociationCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const CaloHitAssociationCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class CaloHitAssociationCollection : public albers::CollectionBase {

public:
  typedef const CaloHitAssociationCollectionIterator const_iterator;

  CaloHitAssociationCollection();
//  CaloHitAssociationCollection(CaloHitAssociationVector* data, int collectionID);
  ~CaloHitAssociationCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  CaloHitAssociationHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const CaloHitAssociationHandle& get(int index) const;

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
  std::vector<CaloHitAssociation>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const CaloHitAssociationHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  CaloHitAssociationVector* m_data;
  CaloHitAssociationHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif