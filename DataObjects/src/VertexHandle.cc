// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/Vertex.h"
#include "DataObjects/VertexCollection.h"



bool  VertexHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
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
