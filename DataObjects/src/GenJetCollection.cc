#include "DataObjects/GenJetCollection.h"

GenJetCollection::GenJetCollection() : m_collectionID(0), m_data(new GenJetVector() ){
}

const GenJetHandle& GenJetCollection::get(int index) const{
  return m_handles[index];
}

GenJetHandle& GenJetCollection::create(){
  m_data->emplace_back(GenJet());
  int index = m_data->size()-1;
  m_handles.emplace_back(GenJetHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void GenJetCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void GenJetCollection::prepareForWrite(const albers::Registry* registry){

}

void GenJetCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(GenJetHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void GenJetCollection::setPODsAddress(const void* address){
  m_data = (GenJetVector*)address;
}


const GenJetHandle GenJetCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void GenJetCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

