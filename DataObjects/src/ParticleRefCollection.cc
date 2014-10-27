#include "DataObjects/ParticleRefCollection.h"

ParticleRefCollection::ParticleRefCollection() : m_collectionID(0), m_data(new ParticleRefVector() ){
}

const ParticleRefHandle& ParticleRefCollection::get(int index) const{
  return m_handles[index];
}

ParticleRefHandle& ParticleRefCollection::create(){
  m_data->emplace_back(ParticleRef());
  int index = m_data->size()-1;
  m_handles.emplace_back(ParticleRefHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void ParticleRefCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void ParticleRefCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Particle.prepareForWrite(registry);
  }
}

void ParticleRefCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  for (auto& data : *m_data){
    data.Particle.prepareAfterRead(registry);

    m_handles.emplace_back(ParticleRefHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void ParticleRefCollection::setPODsAddress(const void* address){
  m_data = (ParticleRefVector*)address;
}


const ParticleRefHandle ParticleRefCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


