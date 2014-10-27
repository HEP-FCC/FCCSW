#ifndef ALBERS_WRITER_H
#define ALBERS_WRITER_H

#include <string>
#include <vector>

// ROOT specific includes
#include "TTree.h"

// forward declarations
class TFile;

namespace albers {
  class CollectionBase;
  class Registry;

  class Writer {

  public:
    Writer(const std::string& filename, Registry* registry);
    ~Writer();

    template<typename T>
    void registerForWrite(const std::string& name);
    void writeEvent();
    void finish();

  private:
    // members
    std::string m_filename;
    Registry* m_registry;
    TFile* m_file;
    TTree* m_datatree;
    TTree* m_metadatatree;
    std::vector<CollectionBase*> m_storedCollections;

  };

template<typename T>
  void Writer::registerForWrite(const std::string& name){
    T* coll = nullptr;
    m_registry->getCollectionFromName(name, coll);
    if(coll==nullptr) {
      std::cerr<<"no such collection to write, throw exception."<<std::endl;
    }
    else {
      m_datatree->Branch(name.c_str(), coll->_getBuffer());
      m_storedCollections.emplace_back(coll);
    }
  }

} //namespace
#endif
