// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/CaloCluster.h"
#include "DataObjects/CaloClusterCollection.h"



bool  CaloClusterHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
  return false;
}

void CaloClusterHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void CaloClusterHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


CaloClusterHandle::CaloClusterHandle(int index, int containerID, std::vector<CaloCluster>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const CaloClusterHandle& p1, const CaloClusterHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
