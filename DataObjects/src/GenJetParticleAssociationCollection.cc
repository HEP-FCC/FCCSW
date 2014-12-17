#include "DataObjects/GenJetParticleAssociationCollection.h"

GenJetParticleAssociationCollection::GenJetParticleAssociationCollection() : m_collectionID(0), m_data(new GenJetParticleAssociationVector() ){
}

const GenJetParticleAssociationHandle& GenJetParticleAssociationCollection::get(int index) const{
  return m_handles[index];
}

GenJetParticleAssociationHandle& GenJetParticleAssociationCollection::create(){
  m_data->emplace_back(GenJetParticleAssociation());
  int index = m_data->size()-1;
  m_handles.emplace_back(GenJetParticleAssociationHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void GenJetParticleAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void GenJetParticleAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Jet.prepareForWrite(registry);
    data.Particle.prepareForWrite(registry);
  }
}

void GenJetParticleAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    data.Jet.prepareAfterRead(registry);
data.Particle.prepareAfterRead(registry);

    m_handles.emplace_back(GenJetParticleAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void GenJetParticleAssociationCollection::setPODsAddress(const void* address){
  m_data = (GenJetParticleAssociationVector*)address;
}


const GenJetParticleAssociationHandle GenJetParticleAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void GenJetParticleAssociationCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

