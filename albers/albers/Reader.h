#ifndef READER_H
#define READER_H

#include <algorithm>
#include <string>
#include <vector>

// forward declarations
class TFile;
class TTree;

#include <iostream>
/*

This class has the function to read available data from disk
and to prepare collections and buffers.
Once data are there it lets the Registry know.

 */


namespace albers {

class EventStore;
class CollectionBase;
class Registry;

class Reader {
  friend EventStore;
  public:
    Reader() : m_eventNumber(0) {}
    ~Reader();
    void openFile(const std::string& filename);
    void closeFile(){};

    // COLIN : unused?
    void readEvent();

    template<typename T>
    bool getCollection(const std::string& name,
		       T*& collection);

    void* getBuffer(const unsigned collectionID);

    Registry* getRegistry() {return m_registry;}

    // COLIN: some of the stuff below should be private
    //  private:
    void readRegistry();
    CollectionBase* readCollection(const std::string& name);

    /// Returns number of entries in the TTree
    unsigned getEntries() const;

    /// Preparing to read next event
    void endOfEvent();

    /// Preparing to read a given event
    void goToEvent(unsigned evnum);

    typedef std::pair<CollectionBase*, std::string> Input;
    std::vector<Input> m_inputs;

    Registry* m_registry;
    TFile* m_file;
    TTree* m_eventTree;
    unsigned m_eventNumber;
};

template<typename T>
bool Reader::getCollection(const std::string& name, T*& collection){
  collection = dynamic_cast<T*>(readCollection(name));
  if (collection != nullptr) {
    return true;
  } else {
    return false;
  }
}

} // namespace

#endif
