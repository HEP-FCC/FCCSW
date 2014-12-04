// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/MCParticleAssociation.h"
#include "DataObjects/MCParticleAssociationCollection.h"



bool  MCParticleAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
  return false;
}

void MCParticleAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void MCParticleAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


MCParticleAssociationHandle::MCParticleAssociationHandle(int index, int containerID, std::vector<MCParticleAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const MCParticleAssociationHandle& p1, const MCParticleAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
