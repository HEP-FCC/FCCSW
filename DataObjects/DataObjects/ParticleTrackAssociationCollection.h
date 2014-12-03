//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ParticleTrackAssociationCollection_H
#define  ParticleTrackAssociationCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/ParticleTrackAssociation.h"
#include "DataObjects/ParticleTrackAssociationHandle.h"

typedef std::vector<ParticleTrackAssociation> ParticleTrackAssociationVector;
typedef std::vector<ParticleTrackAssociationHandle> ParticleTrackAssociationHandleContainer;

class ParticleTrackAssociationCollectionIterator {

  public:
    ParticleTrackAssociationCollectionIterator(int index, const ParticleTrackAssociationCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const ParticleTrackAssociationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ParticleTrackAssociationHandle operator*() const;

    const ParticleTrackAssociationCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const ParticleTrackAssociationCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ParticleTrackAssociationCollection : public albers::CollectionBase {

public:
  typedef const ParticleTrackAssociationCollectionIterator const_iterator;

  ParticleTrackAssociationCollection();
//  ParticleTrackAssociationCollection(ParticleTrackAssociationVector* data, int collectionID);
  ~ParticleTrackAssociationCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  ParticleTrackAssociationHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const ParticleTrackAssociationHandle& get(int index) const;

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
  std::vector<ParticleTrackAssociation>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const ParticleTrackAssociationHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  ParticleTrackAssociationVector* m_data;
  ParticleTrackAssociationHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif