#include "DataObjects/SimCaloClusterHitAssociationCollection.h"

SimCaloClusterHitAssociationCollection::SimCaloClusterHitAssociationCollection() : m_collectionID(0), m_data(new SimCaloClusterHitAssociationVector() ){
}

const SimCaloClusterHitAssociationHandle& SimCaloClusterHitAssociationCollection::get(int index) const{
  return m_handles[index];
}

SimCaloClusterHitAssociationHandle& SimCaloClusterHitAssociationCollection::create(){
  m_data->emplace_back(SimCaloClusterHitAssociation());
  int index = m_data->size()-1;
  m_handles.emplace_back(SimCaloClusterHitAssociationHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void SimCaloClusterHitAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void SimCaloClusterHitAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Cluster.prepareForWrite(registry);
    data.Hit.prepareForWrite(registry);
  }
}

void SimCaloClusterHitAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    data.Cluster.prepareAfterRead(registry);
data.Hit.prepareAfterRead(registry);

    m_handles.emplace_back(SimCaloClusterHitAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void SimCaloClusterHitAssociationCollection::setPODsAddress(const void* address){
  m_data = (SimCaloClusterHitAssociationVector*)address;
}


const SimCaloClusterHitAssociationHandle SimCaloClusterHitAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void SimCaloClusterHitAssociationCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

