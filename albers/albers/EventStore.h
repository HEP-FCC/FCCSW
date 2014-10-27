#ifndef ALBERS_EVENTSTORE_H
#define ALBERS_EVENTSTORE_H

#include <string>
#include <vector>
// albers specific includes
#include "albers/Registry.h"

/**
The event store holds the object collections.

It is used to create new collections, and to access existing ones.
When accessing a collection that is not yet in the event store,
the event store makes use of a Reader to read the collection.

**/

namespace albers {

  class CollectionBase;
  class Reader;
  class Writer;

  class EventStore {

  public:
    /// Collection entry. Each collection is identified by a name
    typedef std::pair<std::string, CollectionBase*> CollPair;

    /// Collection container. COLIN: why not a hash_map?
    typedef std::vector<CollPair> CollContainer;

    EventStore(Registry* registry);
    ~EventStore();

    /// create a new collection
    template<typename T>
      T& create(const std::string& name);

    /// access a collection. returns true if suc
    template<typename T>
      bool get(const std::string& name, T*& collection);

    /// clears all collections. COLIN: choose a different name?
    void next();

    void endOfEvent();

    /// set the reader
    void setReader(Reader* reader);

    bool doGet(const std::string& name, CollectionBase*& collection) const;

  private:
    // members
    mutable CollContainer m_collections;
    Reader* m_reader;
    /// not used at the moment
    Writer* m_writer;
    Registry* m_registry;
  };


template<typename T>
T& EventStore::create(const std::string& name) {
  // TODO: add check for existence
  T* coll = new T();
  m_collections.emplace_back(std::make_pair(name,coll));
  m_registry->registerPOD(coll, name);
  return *coll;
}

template<typename T>
bool EventStore::get(const std::string& name, T*& collection){
  CollectionBase* tmp;
  doGet(name, tmp);

  // a dynamic_cast is used because doGet anyway accesses a container
  // of CollectionBase* -> no need to template deeper code
  collection = dynamic_cast<T*>(tmp);
  if (collection != nullptr) { return true;}
  return false;
}

} //namespace
#endif
