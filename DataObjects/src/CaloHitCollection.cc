#include "DataObjects/CaloHitCollection.h"

CaloHitCollection::CaloHitCollection() : m_collectionID(0), m_data(new CaloHitVector() ){
}

const CaloHitHandle& CaloHitCollection::get(int index) const{
  return m_handles[index];
}

CaloHitHandle& CaloHitCollection::create(){
  m_data->emplace_back(CaloHit());
  int index = m_data->size()-1;
  m_handles.emplace_back(CaloHitHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void CaloHitCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void CaloHitCollection::prepareForWrite(const albers::Registry* registry){

}

void CaloHitCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(CaloHitHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void CaloHitCollection::setPODsAddress(const void* address){
  m_data = (CaloHitVector*)address;
}


const CaloHitHandle CaloHitCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void CaloHitCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

