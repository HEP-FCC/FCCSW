// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/EventInfo.h"
#include "DataObjects/EventInfoCollection.h"

  const int& EventInfoHandle::Number() const { return m_container->at(m_index).Number;}

  void EventInfoHandle::setNumber(int value){ m_container->at(m_index).Number = value;}



bool  EventInfoHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void EventInfoHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void EventInfoHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


EventInfoHandle::EventInfoHandle(int index, int containerID, std::vector<EventInfo>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const EventInfoHandle& p1, const EventInfoHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
