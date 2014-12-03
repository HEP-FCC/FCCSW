// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/TrackStateAssociation.h"
#include "DataObjects/TrackStateAssociationCollection.h"

  const TrackHandle& TrackStateAssociationHandle::Track() const { return m_container->at(m_index).Track;}
  const TrackStateHandle& TrackStateAssociationHandle::State() const { return m_container->at(m_index).State;}

  void TrackStateAssociationHandle::setTrack(TrackHandle value){ m_container->at(m_index).Track = value;}
  void TrackStateAssociationHandle::setState(TrackStateHandle value){ m_container->at(m_index).State = value;}



bool  TrackStateAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void TrackStateAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void TrackStateAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


TrackStateAssociationHandle::TrackStateAssociationHandle(int index, int containerID, std::vector<TrackStateAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const TrackStateAssociationHandle& p1, const TrackStateAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
