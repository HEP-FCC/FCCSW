#include "DataObjects/MCParticleAssociationCollection.h"

MCParticleAssociationCollection::MCParticleAssociationCollection() : m_collectionID(0), m_data(new MCParticleAssociationVector() ){
}

const MCParticleAssociationHandle& MCParticleAssociationCollection::get(int index) const{
  return m_handles[index];
}

MCParticleAssociationHandle& MCParticleAssociationCollection::create(){
  m_data->emplace_back(MCParticleAssociation());
  int index = m_data->size()-1;
  m_handles.emplace_back(MCParticleAssociationHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void MCParticleAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void MCParticleAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Mother.prepareForWrite(registry);
    data.Daughter.prepareForWrite(registry);
  }
}

void MCParticleAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    data.Mother.prepareAfterRead(registry);
data.Daughter.prepareAfterRead(registry);

    m_handles.emplace_back(MCParticleAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void MCParticleAssociationCollection::setPODsAddress(const void* address){
  m_data = (MCParticleAssociationVector*)address;
}


const MCParticleAssociationHandle MCParticleAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void MCParticleAssociationCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

