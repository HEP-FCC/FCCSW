#include "DataObjects/SimCaloClusterCollection.h"

SimCaloClusterCollection::SimCaloClusterCollection() : m_collectionID(0), m_data(new SimCaloClusterVector() ){
}

const SimCaloClusterHandle& SimCaloClusterCollection::get(int index) const{
  return m_handles[index];
}

SimCaloClusterHandle& SimCaloClusterCollection::create(){
  m_data->emplace_back(SimCaloCluster());
  int index = m_data->size()-1;
  m_handles.emplace_back(SimCaloClusterHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void SimCaloClusterCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void SimCaloClusterCollection::prepareForWrite(const albers::Registry* registry){

}

void SimCaloClusterCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(SimCaloClusterHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void SimCaloClusterCollection::setPODsAddress(const void* address){
  m_data = (SimCaloClusterVector*)address;
}


const SimCaloClusterHandle SimCaloClusterCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void SimCaloClusterCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

