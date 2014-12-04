// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/GenJetParticleAssociation.h"
#include "DataObjects/GenJetParticleAssociationCollection.h"

  const GenJetHandle& GenJetParticleAssociationHandle::Jet() const { return m_container->at(m_index).Jet;}
  const MCParticleHandle& GenJetParticleAssociationHandle::Particle() const { return m_container->at(m_index).Particle;}

  void GenJetParticleAssociationHandle::setJet(GenJetHandle value){ m_container->at(m_index).Jet = value;}
  void GenJetParticleAssociationHandle::setParticle(MCParticleHandle value){ m_container->at(m_index).Particle = value;}



bool  GenJetParticleAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void GenJetParticleAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void GenJetParticleAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


GenJetParticleAssociationHandle::GenJetParticleAssociationHandle(int index, int containerID, std::vector<GenJetParticleAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const GenJetParticleAssociationHandle& p1, const GenJetParticleAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
