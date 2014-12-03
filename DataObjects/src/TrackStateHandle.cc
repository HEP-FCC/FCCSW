// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/TrackState.h"
#include "DataObjects/TrackStateCollection.h"

  const float& TrackStateHandle::Location() const { return m_container->at(m_index).Location;}
  const float& TrackStateHandle::Omega() const { return m_container->at(m_index).Omega;}
  const float& TrackStateHandle::D0() const { return m_container->at(m_index).D0;}
  const float& TrackStateHandle::Z0() const { return m_container->at(m_index).Z0;}

  void TrackStateHandle::setLocation(float value){ m_container->at(m_index).Location = value;}
  void TrackStateHandle::setOmega(float value){ m_container->at(m_index).Omega = value;}
  void TrackStateHandle::setD0(float value){ m_container->at(m_index).D0 = value;}
  void TrackStateHandle::setZ0(float value){ m_container->at(m_index).Z0 = value;}



bool  TrackStateHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void TrackStateHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void TrackStateHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


TrackStateHandle::TrackStateHandle(int index, int containerID, std::vector<TrackState>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const TrackStateHandle& p1, const TrackStateHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
