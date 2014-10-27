// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "Particle.h"
#include "ParticleCollection.h"

  const LorentzVector& ParticleHandle::P4() const { return m_container->at(m_index).P4;}
  const int& ParticleHandle::ID() const { return m_container->at(m_index).ID;}
  const int& ParticleHandle::Status() const { return m_container->at(m_index).Status;}

  void ParticleHandle::setP4(LorentzVector value){ m_container->at(m_index).P4 = value;}
  void ParticleHandle::setID(int value){ m_container->at(m_index).ID = value;}
  void ParticleHandle::setStatus(int value){ m_container->at(m_index).Status = value;}


bool  ParticleHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void ParticleHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void ParticleHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


ParticleHandle::ParticleHandle(int index, int containerID, std::vector<Particle>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const ParticleHandle& p1, const ParticleHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
