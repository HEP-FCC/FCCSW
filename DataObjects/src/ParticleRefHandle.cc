// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "ParticleRef.h"
#include "ParticleRefCollection.h"

  const ParticleHandle& ParticleRefHandle::Particle() const { return m_container->at(m_index).Particle;}

  void ParticleRefHandle::setParticle(ParticleHandle value){ m_container->at(m_index).Particle = value;}


bool  ParticleRefHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void ParticleRefHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void ParticleRefHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


ParticleRefHandle::ParticleRefHandle(int index, int containerID, std::vector<ParticleRef>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}
