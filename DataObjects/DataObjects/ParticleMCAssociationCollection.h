//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ParticleMCAssociationCollection_H
#define  ParticleMCAssociationCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/ParticleMCAssociation.h"
#include "DataObjects/ParticleMCAssociationHandle.h"

typedef std::vector<ParticleMCAssociation> ParticleMCAssociationVector;
typedef std::vector<ParticleMCAssociationHandle> ParticleMCAssociationHandleContainer;

class ParticleMCAssociationCollectionIterator {

  public:
    ParticleMCAssociationCollectionIterator(int index, const ParticleMCAssociationCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const ParticleMCAssociationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ParticleMCAssociationHandle operator*() const;

    const ParticleMCAssociationCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const ParticleMCAssociationCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ParticleMCAssociationCollection : public albers::CollectionBase {

public:
  typedef const ParticleMCAssociationCollectionIterator const_iterator;

  ParticleMCAssociationCollection();
//  ParticleMCAssociationCollection(ParticleMCAssociationVector* data, int collectionID);
  ~ParticleMCAssociationCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  ParticleMCAssociationHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const ParticleMCAssociationHandle& get(int index) const;

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
  std::vector<ParticleMCAssociation>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const ParticleMCAssociationHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  ParticleMCAssociationVector* m_data;
  ParticleMCAssociationHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif