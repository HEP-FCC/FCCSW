// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/CaloHit.h"
#include "DataObjects/CaloHitCollection.h"



bool  CaloHitHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
  return false;
}

void CaloHitHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void CaloHitHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


CaloHitHandle::CaloHitHandle(int index, int containerID, std::vector<CaloHit>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const CaloHitHandle& p1, const CaloHitHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
