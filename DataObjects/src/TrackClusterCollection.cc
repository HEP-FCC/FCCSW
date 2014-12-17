#include "DataObjects/TrackClusterCollection.h"

TrackClusterCollection::TrackClusterCollection() : m_collectionID(0), m_data(new TrackClusterVector() ){
}

const TrackClusterHandle& TrackClusterCollection::get(int index) const{
  return m_handles[index];
}

TrackClusterHandle& TrackClusterCollection::create(){
  m_data->emplace_back(TrackCluster());
  int index = m_data->size()-1;
  m_handles.emplace_back(TrackClusterHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void TrackClusterCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void TrackClusterCollection::prepareForWrite(const albers::Registry* registry){

}

void TrackClusterCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(TrackClusterHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void TrackClusterCollection::setPODsAddress(const void* address){
  m_data = (TrackClusterVector*)address;
}


const TrackClusterHandle TrackClusterCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void TrackClusterCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

