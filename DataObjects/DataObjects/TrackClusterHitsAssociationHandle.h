#ifndef TrackClusterHitsAssociationHANDLE_H
#define TrackClusterHitsAssociationHANDLE_H
#include "DataObjects/TrackClusterHitsAssociation.h"
#include "DataObjects/TrackClusterHandle.h"
#include "DataObjects/TrackHitHandle.h"

#include <vector>

// hit handle
// author: C. Bernet, B. Hegner

//forward declaration of TrackClusterHitsAssociation container
class TrackClusterHitsAssociationCollection;

namespace albers {
  class Registry;
}

class TrackClusterHitsAssociationHandle {

  friend class TrackClusterHitsAssociationCollection;

public:

  TrackClusterHitsAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  TrackClusterHitsAssociationHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const TrackClusterHitsAssociation& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  TrackClusterHitsAssociation& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const TrackClusterHitsAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const TrackClusterHitsAssociationHandle& p1,
			 const TrackClusterHitsAssociationHandle& p2 );

private:
  TrackClusterHitsAssociationHandle(int index, int containerID,  std::vector<TrackClusterHitsAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<TrackClusterHitsAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
