// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "DataObjects/VertexTrackAssociation.h"
#include "DataObjects/VertexTrackAssociationCollection.h"

  const TrackHandle& VertexTrackAssociationHandle::Track() const { return m_container->at(m_index).Track;}
  const VertexHandle& VertexTrackAssociationHandle::Vertex() const { return m_container->at(m_index).Vertex;}
  const float& VertexTrackAssociationHandle::Weight() const { return m_container->at(m_index).Weight;}

  void VertexTrackAssociationHandle::setTrack(TrackHandle value){ m_container->at(m_index).Track = value;}
  void VertexTrackAssociationHandle::setVertex(VertexHandle value){ m_container->at(m_index).Vertex = value;}
  void VertexTrackAssociationHandle::setWeight(float value){ m_container->at(m_index).Weight = value;}



bool  VertexTrackAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void VertexTrackAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void VertexTrackAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


VertexTrackAssociationHandle::VertexTrackAssociationHandle(int index, int containerID, std::vector<VertexTrackAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const VertexTrackAssociationHandle& p1, const VertexTrackAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
