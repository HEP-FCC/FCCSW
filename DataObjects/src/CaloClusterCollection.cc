#include "DataObjects/CaloClusterCollection.h"

CaloClusterCollection::CaloClusterCollection() : m_collectionID(0), m_data(new CaloClusterVector() ){
}

const CaloClusterHandle& CaloClusterCollection::get(int index) const{
  return m_handles[index];
}

CaloClusterHandle& CaloClusterCollection::create(){
  m_data->emplace_back(CaloCluster());
  int index = m_data->size()-1;
  m_handles.emplace_back(CaloClusterHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void CaloClusterCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void CaloClusterCollection::prepareForWrite(const albers::Registry* registry){

}

void CaloClusterCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(CaloClusterHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void CaloClusterCollection::setPODsAddress(const void* address){
  m_data = (CaloClusterVector*)address;
}


const CaloClusterHandle CaloClusterCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void CaloClusterCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

