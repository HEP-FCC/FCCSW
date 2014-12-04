#include "DataObjects/TrackClusterHitsAssociationCollection.h"

TrackClusterHitsAssociationCollection::TrackClusterHitsAssociationCollection() : m_collectionID(0), m_data(new TrackClusterHitsAssociationVector() ){
}

const TrackClusterHitsAssociationHandle& TrackClusterHitsAssociationCollection::get(int index) const{
  return m_handles[index];
}

TrackClusterHitsAssociationHandle& TrackClusterHitsAssociationCollection::create(){
  m_data->emplace_back(TrackClusterHitsAssociation());
  int index = m_data->size()-1;
  m_handles.emplace_back(TrackClusterHitsAssociationHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void TrackClusterHitsAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void TrackClusterHitsAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Cluster.prepareForWrite(registry);
    data.Hit.prepareForWrite(registry);
  }
}

void TrackClusterHitsAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    data.Cluster.prepareAfterRead(registry);
data.Hit.prepareAfterRead(registry);

    m_handles.emplace_back(TrackClusterHitsAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void TrackClusterHitsAssociationCollection::setPODsAddress(const void* address){
  m_data = (TrackClusterHitsAssociationVector*)address;
}


const TrackClusterHitsAssociationHandle TrackClusterHitsAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void TrackClusterHitsAssociationCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

