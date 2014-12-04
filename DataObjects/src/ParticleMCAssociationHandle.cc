// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/ParticleMCAssociation.h"
#include "DataObjects/ParticleMCAssociationCollection.h"



bool  ParticleMCAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
  return false;
}

void ParticleMCAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void ParticleMCAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


ParticleMCAssociationHandle::ParticleMCAssociationHandle(int index, int containerID, std::vector<ParticleMCAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const ParticleMCAssociationHandle& p1, const ParticleMCAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
