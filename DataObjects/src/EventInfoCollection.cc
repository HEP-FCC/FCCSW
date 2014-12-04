#include "DataObjects/EventInfoCollection.h"

EventInfoCollection::EventInfoCollection() : m_collectionID(0), m_data(new EventInfoVector() ){
}

const EventInfoHandle& EventInfoCollection::get(int index) const{
  return m_handles[index];
}

EventInfoHandle& EventInfoCollection::create(){
  m_data->emplace_back(EventInfo());
  int index = m_data->size()-1;
  m_handles.emplace_back(EventInfoHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void EventInfoCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void EventInfoCollection::prepareForWrite(const albers::Registry* registry){

}

void EventInfoCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(EventInfoHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void EventInfoCollection::setPODsAddress(const void* address){
  m_data = (EventInfoVector*)address;
}


const EventInfoHandle EventInfoCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void EventInfoCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

