// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/Vertex.h"
#include "DataObjects/VertexCollection.h"

  const float& VertexHandle::Chi2() const { return m_container->at(m_index).Chi2;}
  const unsigned& VertexHandle::Ndf() const { return m_container->at(m_index).Ndf;}
  const Point& VertexHandle::Position() const { return m_container->at(m_index).Position;}
  const unsigned& VertexHandle::Bits() const { return m_container->at(m_index).Bits;}

  void VertexHandle::setChi2(float value){ m_container->at(m_index).Chi2 = value;}
  void VertexHandle::setNdf(unsigned value){ m_container->at(m_index).Ndf = value;}
  void VertexHandle::setPosition(Point value){ m_container->at(m_index).Position = value;}
  void VertexHandle::setBits(unsigned value){ m_container->at(m_index).Bits = value;}



bool  VertexHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void VertexHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void VertexHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


VertexHandle::VertexHandle(int index, int containerID, std::vector<Vertex>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const VertexHandle& p1, const VertexHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
