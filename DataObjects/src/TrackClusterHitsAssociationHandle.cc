// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/TrackClusterHitsAssociation.h"
#include "DataObjects/TrackClusterHitsAssociationCollection.h"

  const TrackClusterHandle& TrackClusterHitsAssociationHandle::Cluster() const { return m_container->at(m_index).Cluster;}
  const TrackHitHandle& TrackClusterHitsAssociationHandle::Hit() const { return m_container->at(m_index).Hit;}

  void TrackClusterHitsAssociationHandle::setCluster(TrackClusterHandle value){ m_container->at(m_index).Cluster = value;}
  void TrackClusterHitsAssociationHandle::setHit(TrackHitHandle value){ m_container->at(m_index).Hit = value;}



bool  TrackClusterHitsAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void TrackClusterHitsAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void TrackClusterHitsAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


TrackClusterHitsAssociationHandle::TrackClusterHitsAssociationHandle(int index, int containerID, std::vector<TrackClusterHitsAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const TrackClusterHitsAssociationHandle& p1, const TrackClusterHitsAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
