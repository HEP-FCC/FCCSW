#include "DataObjects/ParticleMCAssociationCollection.h"

ParticleMCAssociationCollection::ParticleMCAssociationCollection() : m_collectionID(0), m_data(new ParticleMCAssociationVector() ){
}

const ParticleMCAssociationHandle& ParticleMCAssociationCollection::get(int index) const{
  return m_handles[index];
}

ParticleMCAssociationHandle& ParticleMCAssociationCollection::create(){
  m_data->emplace_back(ParticleMCAssociation());
  int index = m_data->size()-1;
  m_handles.emplace_back(ParticleMCAssociationHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void ParticleMCAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void ParticleMCAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Rec.prepareForWrite(registry);
    data.Sim.prepareForWrite(registry);
  }
}

void ParticleMCAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    data.Rec.prepareAfterRead(registry);
data.Sim.prepareAfterRead(registry);

    m_handles.emplace_back(ParticleMCAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void ParticleMCAssociationCollection::setPODsAddress(const void* address){
  m_data = (ParticleMCAssociationVector*)address;
}


const ParticleMCAssociationHandle ParticleMCAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void ParticleMCAssociationCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

