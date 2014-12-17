//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ParticleCollection_H
#define  ParticleCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/Particle.h"
#include "DataObjects/ParticleHandle.h"

typedef std::vector<Particle> ParticleVector;
typedef std::vector<ParticleHandle> ParticleHandleContainer;

class ParticleCollectionIterator {

  public:
    ParticleCollectionIterator(int index, const ParticleCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const ParticleCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ParticleHandle operator*() const;

    const ParticleCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const ParticleCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ParticleCollection : public albers::CollectionBase {

public:
  typedef const ParticleCollectionIterator const_iterator;

  ParticleCollection();
//  ParticleCollection(ParticleVector* data, int collectionID);
  ~ParticleCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  ParticleHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const ParticleHandle& get(int index) const;

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
  std::vector<Particle>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const ParticleHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  ParticleVector* m_data;
  ParticleHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif