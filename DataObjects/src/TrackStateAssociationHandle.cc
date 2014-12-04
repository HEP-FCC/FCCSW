// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/TrackStateAssociation.h"
#include "DataObjects/TrackStateAssociationCollection.h"



bool  TrackStateAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
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
