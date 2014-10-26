#include "DataObjects/DummyDataCollection.h"

DummyDataCollection::DummyDataCollection() : m_collectionID(0), m_data(new DummyDataVector() ){
}

const DummyDataHandle& DummyDataCollection::get(int index) const{
  return m_handles[index];
}

DummyDataHandle& DummyDataCollection::create(){
  m_data->emplace_back(DummyData());
  int index = m_data->size()-1;
  m_handles.emplace_back(DummyDataHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void DummyDataCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void DummyDataCollection::prepareForWrite(const albers::Registry* registry){

}

void DummyDataCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  for (auto& data : *m_data){
    
    m_handles.emplace_back(DummyDataHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void DummyDataCollection::setPODsAddress(const void* address){
  m_data = (DummyDataVector*)address;
}


const DummyDataHandle DummyDataCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


