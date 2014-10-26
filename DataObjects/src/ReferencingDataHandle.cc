// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/ReferencingData.h"
#include "DataObjects/ReferencingDataCollection.h"

  const DummyDataHandle& ReferencingDataHandle::Dummy() const { return m_container->at(m_index).Dummy;}

  void ReferencingDataHandle::setDummy(DummyDataHandle value){ m_container->at(m_index).Dummy = value;}

std::vector<DummyDataHandle>::const_iterator ReferencingDataHandle::Dummies_begin() const {
  auto ret_value = m_Dummies->begin();
  std::advance(ret_value, m_container->at(m_index).Dummies_begin);
  return ret_value;
}

std::vector<DummyDataHandle>::const_iterator ReferencingDataHandle::Dummies_end() const {
  auto ret_value = m_Dummies->begin();
  std::advance(ret_value, m_container->at(m_index).Dummies_end-1);
  return ++ret_value;
}

void ReferencingDataHandle::addDummies(DummyDataHandle& component) {
  m_Dummies->push_back(component);
  m_container->at(m_index).Dummies_end++;
}


bool  ReferencingDataHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void ReferencingDataHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void ReferencingDataHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


ReferencingDataHandle::ReferencingDataHandle(int index, int containerID, std::vector<ReferencingData>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}
