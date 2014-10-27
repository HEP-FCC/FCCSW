//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef JetParticleCollection_H
#define  JetParticleCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DataObjects/JetParticle.h"
#include "DataObjects/JetParticleHandle.h"

typedef std::vector<JetParticle> JetParticleVector;
typedef std::deque<JetParticleHandle> JetParticleHandleContainer;

class JetParticleCollectionIterator {

  public:
    JetParticleCollectionIterator(int index, const JetParticleCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const JetParticleCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const JetParticleHandle operator*() const;

    const JetParticleCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const JetParticleCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class JetParticleCollection : public albers::CollectionBase {

public:
  typedef const JetParticleCollectionIterator const_iterator;

  JetParticleCollection();
//  JetParticleCollection(JetParticleVector* data, int collectionID);
  ~JetParticleCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  JetParticleHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const JetParticleHandle& get(int index) const;

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
  std::vector<JetParticle>* _getBuffer(){ return m_data;};
private:
  unsigned m_collectionID;
  JetParticleVector* m_data;
  JetParticleHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif