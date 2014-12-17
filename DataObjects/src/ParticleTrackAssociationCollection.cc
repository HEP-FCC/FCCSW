#include "DataObjects/ParticleTrackAssociationCollection.h"

ParticleTrackAssociationCollection::ParticleTrackAssociationCollection() : m_collectionID(0), m_data(new ParticleTrackAssociationVector() ){
}

const ParticleTrackAssociationHandle& ParticleTrackAssociationCollection::get(int index) const{
  return m_handles[index];
}

ParticleTrackAssociationHandle& ParticleTrackAssociationCollection::create(){
  m_data->emplace_back(ParticleTrackAssociation());
  int index = m_data->size()-1;
  m_handles.emplace_back(ParticleTrackAssociationHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void ParticleTrackAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void ParticleTrackAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Particle.prepareForWrite(registry);
    data.Track.prepareForWrite(registry);
  }
}

void ParticleTrackAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    data.Particle.prepareAfterRead(registry);
data.Track.prepareAfterRead(registry);

    m_handles.emplace_back(ParticleTrackAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void ParticleTrackAssociationCollection::setPODsAddress(const void* address){
  m_data = (ParticleTrackAssociationVector*)address;
}


const ParticleTrackAssociationHandle ParticleTrackAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void ParticleTrackAssociationCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

