// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/MissingEnergy.h"
#include "DataObjects/MissingEnergyCollection.h"

  const float& MissingEnergyHandle::Energy() const { return m_container->at(m_index).Energy;}
  const float& MissingEnergyHandle::Phi() const { return m_container->at(m_index).Phi;}

  void MissingEnergyHandle::setEnergy(float value){ m_container->at(m_index).Energy = value;}
  void MissingEnergyHandle::setPhi(float value){ m_container->at(m_index).Phi = value;}



bool  MissingEnergyHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void MissingEnergyHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void MissingEnergyHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


MissingEnergyHandle::MissingEnergyHandle(int index, int containerID, std::vector<MissingEnergy>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const MissingEnergyHandle& p1, const MissingEnergyHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
