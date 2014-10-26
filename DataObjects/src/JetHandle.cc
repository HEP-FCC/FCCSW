// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/Jet.h"
#include "DataObjects/JetCollection.h"

  const LorentzVector& JetHandle::P4() const { return m_container->at(m_index).P4;}

  void JetHandle::setP4(LorentzVector value){ m_container->at(m_index).P4 = value;}

std::vector<ParticleHandle>::const_iterator JetHandle::particles_begin() const {
  auto ret_value = m_particles->begin();
  std::advance(ret_value, m_container->at(m_index).particles_begin);
  return ret_value;
}

std::vector<ParticleHandle>::const_iterator JetHandle::particles_end() const {
  auto ret_value = m_particles->begin();
  std::advance(ret_value, m_container->at(m_index).particles_end-1);
  return ++ret_value;
}

void JetHandle::addparticles(ParticleHandle& component) {
  m_particles->push_back(component);
  m_container->at(m_index).particles_end++;
}


bool  JetHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void JetHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void JetHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


JetHandle::JetHandle(int index, int containerID, std::vector<Jet>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}
