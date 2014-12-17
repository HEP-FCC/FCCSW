#ifndef ParticleTrackAssociationHANDLE_H
#define ParticleTrackAssociationHANDLE_H
#include "DataObjects/ParticleTrackAssociation.h"
#include "DataObjects/ParticleHandle.h"
#include "DataObjects/TrackHandle.h"

#include <vector>

// The track
// author: C. Bernet, B. Hegner

//forward declaration of ParticleTrackAssociation container
class ParticleTrackAssociationCollection;

namespace albers {
  class Registry;
}

class ParticleTrackAssociationHandle {

  friend class ParticleTrackAssociationCollection;

public:

  ParticleTrackAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  ParticleTrackAssociationHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const ParticleTrackAssociation& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  ParticleTrackAssociation& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const ParticleTrackAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const ParticleTrackAssociationHandle& p1,
			 const ParticleTrackAssociationHandle& p2 );

private:
  ParticleTrackAssociationHandle(int index, int containerID,  std::vector<ParticleTrackAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<ParticleTrackAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
