
// ROOT specifc includes
#include "TFile.h"
#include "TTree.h"

// albers specific includes
#include "albers/CollectionBase.h"
#include "albers/Registry.h"
#include "albers/Writer.h"

namespace albers {

  Writer::Writer(const std::string& filename, Registry* registry) :
    m_filename(filename),
    m_registry(registry)
  {
    m_file = new TFile(filename.c_str(),"RECREATE","data file");
    m_datatree  = new TTree("events","Events tree");
    m_metadatatree = new TTree("metadata", "Metadata tree");
  }

  Writer::~Writer(){
    delete m_file;
  }

  void Writer::writeEvent(){
    for (auto& coll : m_storedCollections){
      coll->prepareForWrite(m_registry);
    }
    m_datatree->Fill();
    // m_file->Write();
  }

  void Writer::finish(){
    // now we want to safe the metadata
    m_metadatatree->Branch("Registry",m_registry);
    m_metadatatree->Fill();
    m_file->Write();
    m_file->Close();
  }


} // namespace
