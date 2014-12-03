#include "DataObjects/SimCaloHitMCParticleAssociationCollection.h"

SimCaloHitMCParticleAssociationCollection::SimCaloHitMCParticleAssociationCollection() : m_collectionID(0), m_data(new SimCaloHitMCParticleAssociationVector() ){
}

const SimCaloHitMCParticleAssociationHandle& SimCaloHitMCParticleAssociationCollection::get(int index) const{
  return m_handles[index];
}

SimCaloHitMCParticleAssociationHandle& SimCaloHitMCParticleAssociationCollection::create(){
  m_data->emplace_back(SimCaloHitMCParticleAssociation());
  int index = m_data->size()-1;
  m_handles.emplace_back(SimCaloHitMCParticleAssociationHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void SimCaloHitMCParticleAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void SimCaloHitMCParticleAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Hit.prepareForWrite(registry);
    data.Particle.prepareForWrite(registry);
  }
}

void SimCaloHitMCParticleAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    data.Hit.prepareAfterRead(registry);
data.Particle.prepareAfterRead(registry);

    m_handles.emplace_back(SimCaloHitMCParticleAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void SimCaloHitMCParticleAssociationCollection::setPODsAddress(const void* address){
  m_data = (SimCaloHitMCParticleAssociationVector*)address;
}


const SimCaloHitMCParticleAssociationHandle SimCaloHitMCParticleAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void SimCaloHitMCParticleAssociationCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

