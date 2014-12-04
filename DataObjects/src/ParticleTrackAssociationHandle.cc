// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/ParticleTrackAssociation.h"
#include "DataObjects/ParticleTrackAssociationCollection.h"

  const ParticleHandle& ParticleTrackAssociationHandle::Particle() const { return m_container->at(m_index).Particle;}
  const TrackHandle& ParticleTrackAssociationHandle::Track() const { return m_container->at(m_index).Track;}

  void ParticleTrackAssociationHandle::setParticle(ParticleHandle value){ m_container->at(m_index).Particle = value;}
  void ParticleTrackAssociationHandle::setTrack(TrackHandle value){ m_container->at(m_index).Track = value;}



bool  ParticleTrackAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void ParticleTrackAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void ParticleTrackAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


ParticleTrackAssociationHandle::ParticleTrackAssociationHandle(int index, int containerID, std::vector<ParticleTrackAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const ParticleTrackAssociationHandle& p1, const ParticleTrackAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
