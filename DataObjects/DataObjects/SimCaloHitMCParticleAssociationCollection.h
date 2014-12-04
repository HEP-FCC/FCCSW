//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef SimCaloHitMCParticleAssociationCollection_H
#define  SimCaloHitMCParticleAssociationCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/SimCaloHitMCParticleAssociation.h"
#include "DataObjects/SimCaloHitMCParticleAssociationHandle.h"

typedef std::vector<SimCaloHitMCParticleAssociation> SimCaloHitMCParticleAssociationVector;
typedef std::vector<SimCaloHitMCParticleAssociationHandle> SimCaloHitMCParticleAssociationHandleContainer;

class SimCaloHitMCParticleAssociationCollectionIterator {

  public:
    SimCaloHitMCParticleAssociationCollectionIterator(int index, const SimCaloHitMCParticleAssociationCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const SimCaloHitMCParticleAssociationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const SimCaloHitMCParticleAssociationHandle operator*() const;

    const SimCaloHitMCParticleAssociationCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const SimCaloHitMCParticleAssociationCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class SimCaloHitMCParticleAssociationCollection : public albers::CollectionBase {

public:
  typedef const SimCaloHitMCParticleAssociationCollectionIterator const_iterator;

  SimCaloHitMCParticleAssociationCollection();
//  SimCaloHitMCParticleAssociationCollection(SimCaloHitMCParticleAssociationVector* data, int collectionID);
  ~SimCaloHitMCParticleAssociationCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  SimCaloHitMCParticleAssociationHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const SimCaloHitMCParticleAssociationHandle& get(int index) const;

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
  std::vector<SimCaloHitMCParticleAssociation>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const SimCaloHitMCParticleAssociationHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  SimCaloHitMCParticleAssociationVector* m_data;
  SimCaloHitMCParticleAssociationHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif