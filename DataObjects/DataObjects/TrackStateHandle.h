#ifndef TrackStateHANDLE_H
#define TrackStateHANDLE_H
#include "DataObjects/TrackState.h"

#include <vector>

// Longitudinal impact parameter
// author: C. Bernet, B. Hegner

//forward declaration of TrackState container
class TrackStateCollection;

namespace albers {
  class Registry;
}

class TrackStateHandle {

  friend class TrackStateCollection;

public:

  TrackStateHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  TrackStateHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const TrackState& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  TrackState& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const TrackStateHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const TrackStateHandle& p1,
			 const TrackStateHandle& p2 );

private:
  TrackStateHandle(int index, int containerID,  std::vector<TrackState>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<TrackState>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
