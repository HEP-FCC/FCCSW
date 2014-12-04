// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/TrackCluster.h"
#include "DataObjects/TrackClusterCollection.h"



bool  TrackClusterHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
  return false;
}

void TrackClusterHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void TrackClusterHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


TrackClusterHandle::TrackClusterHandle(int index, int containerID, std::vector<TrackCluster>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const TrackClusterHandle& p1, const TrackClusterHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
