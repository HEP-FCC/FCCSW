//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef MCParticleCollection_H
#define  MCParticleCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/MCParticle.h"
#include "DataObjects/MCParticleHandle.h"

typedef std::vector<MCParticle> MCParticleVector;
typedef std::vector<MCParticleHandle> MCParticleHandleContainer;

class MCParticleCollectionIterator {

  public:
    MCParticleCollectionIterator(int index, const MCParticleCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const MCParticleCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const MCParticleHandle operator*() const;

    const MCParticleCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const MCParticleCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class MCParticleCollection : public albers::CollectionBase {

public:
  typedef const MCParticleCollectionIterator const_iterator;

  MCParticleCollection();
//  MCParticleCollection(MCParticleVector* data, int collectionID);
  ~MCParticleCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  MCParticleHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const MCParticleHandle& get(int index) const;

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
  std::vector<MCParticle>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const MCParticleHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  MCParticleVector* m_data;
  MCParticleHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif