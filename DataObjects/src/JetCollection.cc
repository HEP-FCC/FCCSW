#include "DataObjects/JetCollection.h"

JetCollection::JetCollection() : m_collectionID(0), m_data(new JetVector() ){
}

const JetHandle& JetCollection::get(int index) const{
  return m_handles[index];
}

JetHandle& JetCollection::create(){
  m_data->emplace_back(Jet());
  int index = m_data->size()-1;
  m_handles.emplace_back(JetHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void JetCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void JetCollection::prepareForWrite(const albers::Registry* registry){

}

void JetCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(JetHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void JetCollection::setPODsAddress(const void* address){
  m_data = (JetVector*)address;
}


const JetHandle JetCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void JetCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

