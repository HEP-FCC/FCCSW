#include "DataObjects/JetCollection.h"

JetCollection::JetCollection() : m_collectionID(0), m_data(new JetVector() ){
}

const JetHandle& JetCollection::get(int index) const{
  return m_handles[index];
}

JetHandle& JetCollection::create(){
  m_data->emplace_back(Jet());
  int index = m_data->size()-1;
  m_handles.emplace_back(JetHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();
  auto particles_tmp = new std::vector<ParticleHandle>();
  m_rel_particles_tmp.push_back(particles_tmp);
  tmp_handle.m_particles = particles_tmp;

  return tmp_handle;
}

void JetCollection::clear(){
  m_data->clear();
  m_handles.clear();
  for (auto& pointer : m_rel_particles_tmp) {delete pointer;}
  m_rel_particles_tmp.clear();
  m_rel_particles->clear();

}

void JetCollection::prepareForWrite(const albers::Registry* registry){

}

void JetCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  for (auto& data : *m_data){
    
    m_handles.emplace_back(JetHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void JetCollection::setPODsAddress(const void* address){
  m_data = (JetVector*)address;
}


const JetHandle JetCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


