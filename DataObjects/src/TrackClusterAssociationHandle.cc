// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/TrackClusterAssociation.h"
#include "DataObjects/TrackClusterAssociationCollection.h"

  const TrackHandle& TrackClusterAssociationHandle::Track() const { return m_container->at(m_index).Track;}
  const TrackClusterHandle& TrackClusterAssociationHandle::Cluster() const { return m_container->at(m_index).Cluster;}

  void TrackClusterAssociationHandle::setTrack(TrackHandle value){ m_container->at(m_index).Track = value;}
  void TrackClusterAssociationHandle::setCluster(TrackClusterHandle value){ m_container->at(m_index).Cluster = value;}



bool  TrackClusterAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void TrackClusterAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void TrackClusterAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


TrackClusterAssociationHandle::TrackClusterAssociationHandle(int index, int containerID, std::vector<TrackClusterAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const TrackClusterAssociationHandle& p1, const TrackClusterAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
