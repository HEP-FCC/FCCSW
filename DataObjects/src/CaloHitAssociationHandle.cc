// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/CaloHitAssociation.h"
#include "DataObjects/CaloHitAssociationCollection.h"

  const CaloHitHandle& CaloHitAssociationHandle::Rec() const { return m_container->at(m_index).Rec;}
  const SimCaloHitHandle& CaloHitAssociationHandle::Sim() const { return m_container->at(m_index).Sim;}

  void CaloHitAssociationHandle::setRec(CaloHitHandle value){ m_container->at(m_index).Rec = value;}
  void CaloHitAssociationHandle::setSim(SimCaloHitHandle value){ m_container->at(m_index).Sim = value;}



bool  CaloHitAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void CaloHitAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void CaloHitAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


CaloHitAssociationHandle::CaloHitAssociationHandle(int index, int containerID, std::vector<CaloHitAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const CaloHitAssociationHandle& p1, const CaloHitAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
