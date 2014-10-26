#include "DataObjects/ReferencingDataCollection.h"

ReferencingDataCollection::ReferencingDataCollection() : m_collectionID(0), m_data(new ReferencingDataVector() ){
}

const ReferencingDataHandle& ReferencingDataCollection::get(int index) const{
  return m_handles[index];
}

ReferencingDataHandle& ReferencingDataCollection::create(){
  m_data->emplace_back(ReferencingData());
  int index = m_data->size()-1;
  m_handles.emplace_back(ReferencingDataHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();
  auto Dummies_tmp = new std::vector<DummyDataHandle>();
  m_rel_Dummies_tmp.push_back(Dummies_tmp);
  tmp_handle.m_Dummies = Dummies_tmp;

  return tmp_handle;
}

void ReferencingDataCollection::clear(){
  m_data->clear();
  m_handles.clear();
  for (auto& pointer : m_rel_Dummies_tmp) {delete pointer;}
  m_rel_Dummies_tmp.clear();
  m_rel_Dummies->clear();

}

void ReferencingDataCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Dummy.prepareForWrite(registry);
  }
}

void ReferencingDataCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  for (auto& data : *m_data){
    data.Dummy.prepareAfterRead(registry);

    m_handles.emplace_back(ReferencingDataHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void ReferencingDataCollection::setPODsAddress(const void* address){
  m_data = (ReferencingDataVector*)address;
}


const ReferencingDataHandle ReferencingDataCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


