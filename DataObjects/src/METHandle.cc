// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/MET.h"
#include "DataObjects/METCollection.h"

  const float& METHandle::Pt() const { return m_container->at(m_index).Pt;}
  const float& METHandle::Phi() const { return m_container->at(m_index).Phi;}

  void METHandle::setPt(float value){ m_container->at(m_index).Pt = value;}
  void METHandle::setPhi(float value){ m_container->at(m_index).Phi = value;}



bool  METHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
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
