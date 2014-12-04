// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/SimCaloHitMCParticleAssociation.h"
#include "DataObjects/SimCaloHitMCParticleAssociationCollection.h"

  const SimCaloHitHandle& SimCaloHitMCParticleAssociationHandle::Hit() const { return m_container->at(m_index).Hit;}
  const MCParticleHandle& SimCaloHitMCParticleAssociationHandle::Particle() const { return m_container->at(m_index).Particle;}
  const float& SimCaloHitMCParticleAssociationHandle::Fraction() const { return m_container->at(m_index).Fraction;}

  void SimCaloHitMCParticleAssociationHandle::setHit(SimCaloHitHandle value){ m_container->at(m_index).Hit = value;}
  void SimCaloHitMCParticleAssociationHandle::setParticle(MCParticleHandle value){ m_container->at(m_index).Particle = value;}
  void SimCaloHitMCParticleAssociationHandle::setFraction(float value){ m_container->at(m_index).Fraction = value;}



bool  SimCaloHitMCParticleAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void SimCaloHitMCParticleAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void SimCaloHitMCParticleAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


SimCaloHitMCParticleAssociationHandle::SimCaloHitMCParticleAssociationHandle(int index, int containerID, std::vector<SimCaloHitMCParticleAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const SimCaloHitMCParticleAssociationHandle& p1, const SimCaloHitMCParticleAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
