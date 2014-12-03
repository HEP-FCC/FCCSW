//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef GenJetParticleAssociationCollection_H
#define  GenJetParticleAssociationCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/GenJetParticleAssociation.h"
#include "DataObjects/GenJetParticleAssociationHandle.h"

typedef std::vector<GenJetParticleAssociation> GenJetParticleAssociationVector;
typedef std::vector<GenJetParticleAssociationHandle> GenJetParticleAssociationHandleContainer;

class GenJetParticleAssociationCollectionIterator {

  public:
    GenJetParticleAssociationCollectionIterator(int index, const GenJetParticleAssociationCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const GenJetParticleAssociationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const GenJetParticleAssociationHandle operator*() const;

    const GenJetParticleAssociationCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const GenJetParticleAssociationCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class GenJetParticleAssociationCollection : public albers::CollectionBase {

public:
  typedef const GenJetParticleAssociationCollectionIterator const_iterator;

  GenJetParticleAssociationCollection();
//  GenJetParticleAssociationCollection(GenJetParticleAssociationVector* data, int collectionID);
  ~GenJetParticleAssociationCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  GenJetParticleAssociationHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const GenJetParticleAssociationHandle& get(int index) const;

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
  std::vector<GenJetParticleAssociation>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const GenJetParticleAssociationHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  GenJetParticleAssociationVector* m_data;
  GenJetParticleAssociationHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif