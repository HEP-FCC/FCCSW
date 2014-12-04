#ifndef VertexTrackAssociationHANDLE_H
#define VertexTrackAssociationHANDLE_H
#include "DataObjects/VertexTrackAssociation.h"
#include "DataObjects/TrackHandle.h"
#include "DataObjects/VertexHandle.h"

#include <vector>

// Weight of the track in the vertex.
// author: C. Bernet, B. Hegner

//forward declaration of VertexTrackAssociation container
class VertexTrackAssociationCollection;

namespace albers {
  class Registry;
}

class VertexTrackAssociationHandle {

  friend class VertexTrackAssociationCollection;

public:

VertexTrackAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  VertexTrackAssociationHandle() = default;

  const TrackHandle& Track() const;
  const VertexHandle& Vertex() const;
  const float& Weight() const;

  void setTrack(TrackHandle value);
  void setVertex(VertexHandle value);
  void setWeight(float value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const VertexTrackAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const VertexTrackAssociationHandle& p1,
			 const VertexTrackAssociationHandle& p2 );

private:
  VertexTrackAssociationHandle(int index, int containerID,  std::vector<VertexTrackAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<VertexTrackAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
