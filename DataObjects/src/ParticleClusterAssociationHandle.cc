// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/ParticleClusterAssociation.h"
#include "DataObjects/ParticleClusterAssociationCollection.h"

  const ParticleHandle& ParticleClusterAssociationHandle::Particle() const { return m_container->at(m_index).Particle;}
  const CaloClusterHandle& ParticleClusterAssociationHandle::Cluster() const { return m_container->at(m_index).Cluster;}

  void ParticleClusterAssociationHandle::setParticle(ParticleHandle value){ m_container->at(m_index).Particle = value;}
  void ParticleClusterAssociationHandle::setCluster(CaloClusterHandle value){ m_container->at(m_index).Cluster = value;}



bool  ParticleClusterAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void ParticleClusterAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void ParticleClusterAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


ParticleClusterAssociationHandle::ParticleClusterAssociationHandle(int index, int containerID, std::vector<ParticleClusterAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const ParticleClusterAssociationHandle& p1, const ParticleClusterAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
