#include "DataObjects/CaloHitAssociationCollection.h"

CaloHitAssociationCollection::CaloHitAssociationCollection() : m_collectionID(0), m_data(new CaloHitAssociationVector() ){
}

const CaloHitAssociationHandle& CaloHitAssociationCollection::get(int index) const{
  return m_handles[index];
}

CaloHitAssociationHandle& CaloHitAssociationCollection::create(){
  m_data->emplace_back(CaloHitAssociation());
  int index = m_data->size()-1;
  m_handles.emplace_back(CaloHitAssociationHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void CaloHitAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void CaloHitAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Rec.prepareForWrite(registry);
    data.Sim.prepareForWrite(registry);
  }
}

void CaloHitAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    data.Rec.prepareAfterRead(registry);
data.Sim.prepareAfterRead(registry);

    m_handles.emplace_back(CaloHitAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void CaloHitAssociationCollection::setPODsAddress(const void* address){
  m_data = (CaloHitAssociationVector*)address;
}


const CaloHitAssociationHandle CaloHitAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void CaloHitAssociationCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

