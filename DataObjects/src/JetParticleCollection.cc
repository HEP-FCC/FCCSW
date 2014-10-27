#include "DataObjects/JetParticleCollection.h"

JetParticleCollection::JetParticleCollection() : m_collectionID(0), m_data(new JetParticleVector() ){
}

const JetParticleHandle& JetParticleCollection::get(int index) const{
  return m_handles[index];
}

JetParticleHandle& JetParticleCollection::create(){
  m_data->emplace_back(JetParticle());
  int index = m_data->size()-1;
  m_handles.emplace_back(JetParticleHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void JetParticleCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void JetParticleCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Jet.prepareForWrite(registry);
    data.Particle.prepareForWrite(registry);
  }
}

void JetParticleCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  for (auto& data : *m_data){
    data.Jet.prepareAfterRead(registry);
data.Particle.prepareAfterRead(registry);

    m_handles.emplace_back(JetParticleHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void JetParticleCollection::setPODsAddress(const void* address){
  m_data = (JetParticleVector*)address;
}


const JetParticleHandle JetParticleCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


