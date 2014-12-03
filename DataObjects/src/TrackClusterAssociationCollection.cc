#include "DataObjects/TrackClusterAssociationCollection.h"

TrackClusterAssociationCollection::TrackClusterAssociationCollection() : m_collectionID(0), m_data(new TrackClusterAssociationVector() ){
}

const TrackClusterAssociationHandle& TrackClusterAssociationCollection::get(int index) const{
  return m_handles[index];
}

TrackClusterAssociationHandle& TrackClusterAssociationCollection::create(){
  m_data->emplace_back(TrackClusterAssociation());
  int index = m_data->size()-1;
  m_handles.emplace_back(TrackClusterAssociationHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void TrackClusterAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void TrackClusterAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Track.prepareForWrite(registry);
    data.Cluster.prepareForWrite(registry);
  }
}

void TrackClusterAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    data.Track.prepareAfterRead(registry);
data.Cluster.prepareAfterRead(registry);

    m_handles.emplace_back(TrackClusterAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void TrackClusterAssociationCollection::setPODsAddress(const void* address){
  m_data = (TrackClusterAssociationVector*)address;
}


const TrackClusterAssociationHandle TrackClusterAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void TrackClusterAssociationCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

