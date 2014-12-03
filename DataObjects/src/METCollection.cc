#include "DataObjects/METCollection.h"

METCollection::METCollection() : m_collectionID(0), m_data(new METVector() ){
}

const METHandle& METCollection::get(int index) const{
  return m_handles[index];
}

METHandle& METCollection::create(){
  m_data->emplace_back(MET());
  int index = m_data->size()-1;
  m_handles.emplace_back(METHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void METCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void METCollection::prepareForWrite(const albers::Registry* registry){

}

void METCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(METHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void METCollection::setPODsAddress(const void* address){
  m_data = (METVector*)address;
}


const METHandle METCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void METCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

