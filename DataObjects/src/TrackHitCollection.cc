#include "DataObjects/TrackHitCollection.h"

TrackHitCollection::TrackHitCollection() : m_collectionID(0), m_data(new TrackHitVector() ){
}

const TrackHitHandle& TrackHitCollection::get(int index) const{
  return m_handles[index];
}

TrackHitHandle& TrackHitCollection::create(){
  m_data->emplace_back(TrackHit());
  int index = m_data->size()-1;
  m_handles.emplace_back(TrackHitHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void TrackHitCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void TrackHitCollection::prepareForWrite(const albers::Registry* registry){

}

void TrackHitCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(TrackHitHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void TrackHitCollection::setPODsAddress(const void* address){
  m_data = (TrackHitVector*)address;
}


const TrackHitHandle TrackHitCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void TrackHitCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

