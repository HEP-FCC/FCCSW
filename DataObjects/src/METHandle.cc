// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/MET.h"
#include "DataObjects/METCollection.h"



bool  METHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
  return false;
}

void METHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void METHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


METHandle::METHandle(int index, int containerID, std::vector<MET>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const METHandle& p1, const METHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
