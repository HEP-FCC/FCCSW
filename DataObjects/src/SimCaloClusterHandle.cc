// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/SimCaloCluster.h"
#include "DataObjects/SimCaloClusterCollection.h"

  const BareCluster& SimCaloClusterHandle::Core() const { return m_container->at(m_index).Core;}

  void SimCaloClusterHandle::setCore(BareCluster value){ m_container->at(m_index).Core = value;}



bool  SimCaloClusterHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void SimCaloClusterHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void SimCaloClusterHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


SimCaloClusterHandle::SimCaloClusterHandle(int index, int containerID, std::vector<SimCaloCluster>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const SimCaloClusterHandle& p1, const SimCaloClusterHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
