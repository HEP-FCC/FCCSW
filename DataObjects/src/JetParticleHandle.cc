// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/JetParticle.h"
#include "DataObjects/JetParticleCollection.h"

  const JetHandle& JetParticleHandle::Jet() const { return m_container->at(m_index).Jet;}
  const ParticleHandle& JetParticleHandle::Particle() const { return m_container->at(m_index).Particle;}

  void JetParticleHandle::setJet(JetHandle value){ m_container->at(m_index).Jet = value;}
  void JetParticleHandle::setParticle(ParticleHandle value){ m_container->at(m_index).Particle = value;}



bool  JetParticleHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void JetParticleHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void JetParticleHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


JetParticleHandle::JetParticleHandle(int index, int containerID, std::vector<JetParticle>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}
