// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/Track.h"
#include "DataObjects/TrackCollection.h"

  const float& TrackHandle::Chi2() const { return m_container->at(m_index).Chi2;}
  const unsigned& TrackHandle::Ndf() const { return m_container->at(m_index).Ndf;}
  const unsigned& TrackHandle::Bits() const { return m_container->at(m_index).Bits;}

  void TrackHandle::setChi2(float value){ m_container->at(m_index).Chi2 = value;}
  void TrackHandle::setNdf(unsigned value){ m_container->at(m_index).Ndf = value;}
  void TrackHandle::setBits(unsigned value){ m_container->at(m_index).Bits = value;}



bool  TrackHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void TrackHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void TrackHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


TrackHandle::TrackHandle(int index, int containerID, std::vector<Track>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const TrackHandle& p1, const TrackHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
