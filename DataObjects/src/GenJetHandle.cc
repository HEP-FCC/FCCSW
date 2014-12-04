// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/GenJet.h"
#include "DataObjects/GenJetCollection.h"



bool  GenJetHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
  return false;
}

void GenJetHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void GenJetHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


GenJetHandle::GenJetHandle(int index, int containerID, std::vector<GenJet>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const GenJetHandle& p1, const GenJetHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
