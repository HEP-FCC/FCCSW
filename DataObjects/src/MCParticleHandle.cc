// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/MCParticle.h"
#include "DataObjects/MCParticleCollection.h"

  const BareParticle& MCParticleHandle::Core() const { return m_container->at(m_index).Core;}
  const Point& MCParticleHandle::Endvertex() const { return m_container->at(m_index).Endvertex;}

  void MCParticleHandle::setCore(BareParticle value){ m_container->at(m_index).Core = value;}
  void MCParticleHandle::setEndvertex(Point value){ m_container->at(m_index).Endvertex = value;}



bool  MCParticleHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void MCParticleHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void MCParticleHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


MCParticleHandle::MCParticleHandle(int index, int containerID, std::vector<MCParticle>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const MCParticleHandle& p1, const MCParticleHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
