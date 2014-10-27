//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ParticleRefCollection_H
#define  ParticleRefCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/ParticleRef.h"
#include "DataObjects/ParticleRefHandle.h"

typedef std::vector<ParticleRef> ParticleRefVector;
typedef std::deque<ParticleRefHandle> ParticleRefHandleContainer;

class ParticleRefCollectionIterator {

  public:
    ParticleRefCollectionIterator(int index, const ParticleRefCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const ParticleRefCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ParticleRefHandle operator*() const;

    const ParticleRefCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const ParticleRefCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ParticleRefCollection : public albers::CollectionBase {

public:
  typedef const ParticleRefCollectionIterator const_iterator;

  ParticleRefCollection();
//  ParticleRefCollection(ParticleRefVector* data, int collectionID);
  ~ParticleRefCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  ParticleRefHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const ParticleRefHandle& get(int index) const;

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
  std::vector<ParticleRef>* _getBuffer(){ return m_data;};
private:
  unsigned m_collectionID;
  ParticleRefVector* m_data;
  ParticleRefHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif