// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/JetParticleAssociation.h"
#include "DataObjects/JetParticleAssociationCollection.h"



bool  JetParticleAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
  return false;
}

void JetParticleAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void JetParticleAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


JetParticleAssociationHandle::JetParticleAssociationHandle(int index, int containerID, std::vector<JetParticleAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const JetParticleAssociationHandle& p1, const JetParticleAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
