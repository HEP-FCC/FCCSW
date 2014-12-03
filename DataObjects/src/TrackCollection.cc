#include "DataObjects/TrackCollection.h"

TrackCollection::TrackCollection() : m_collectionID(0), m_data(new TrackVector() ){
}

const TrackHandle& TrackCollection::get(int index) const{
  return m_handles[index];
}

TrackHandle& TrackCollection::create(){
  m_data->emplace_back(Track());
  int index = m_data->size()-1;
  m_handles.emplace_back(TrackHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void TrackCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void TrackCollection::prepareForWrite(const albers::Registry* registry){

}

void TrackCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(TrackHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void TrackCollection::setPODsAddress(const void* address){
  m_data = (TrackVector*)address;
}


const TrackHandle TrackCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void TrackCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

