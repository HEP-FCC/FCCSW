// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/SimCaloHit.h"
#include "DataObjects/SimCaloHitCollection.h"



bool  SimCaloHitHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
  return false;
}

void SimCaloHitHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void SimCaloHitHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


SimCaloHitHandle::SimCaloHitHandle(int index, int containerID, std::vector<SimCaloHit>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const SimCaloHitHandle& p1, const SimCaloHitHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
