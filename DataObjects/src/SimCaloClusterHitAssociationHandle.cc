// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/SimCaloClusterHitAssociation.h"
#include "DataObjects/SimCaloClusterHitAssociationCollection.h"



bool  SimCaloClusterHitAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
  return false;
}

void SimCaloClusterHitAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void SimCaloClusterHitAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


SimCaloClusterHitAssociationHandle::SimCaloClusterHitAssociationHandle(int index, int containerID, std::vector<SimCaloClusterHitAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const SimCaloClusterHitAssociationHandle& p1, const SimCaloClusterHitAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
