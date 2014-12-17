#include "DataObjects/TrackStateCollection.h"

TrackStateCollection::TrackStateCollection() : m_collectionID(0), m_data(new TrackStateVector() ){
}

const TrackStateHandle& TrackStateCollection::get(int index) const{
  return m_handles[index];
}

TrackStateHandle& TrackStateCollection::create(){
  m_data->emplace_back(TrackState());
  int index = m_data->size()-1;
  m_handles.emplace_back(TrackStateHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void TrackStateCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void TrackStateCollection::prepareForWrite(const albers::Registry* registry){

}

void TrackStateCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(TrackStateHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void TrackStateCollection::setPODsAddress(const void* address){
  m_data = (TrackStateVector*)address;
}


const TrackStateHandle TrackStateCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void TrackStateCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

