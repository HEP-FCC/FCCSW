#ifndef TrackHANDLE_H
#define TrackHANDLE_H
#include "DataObjects/Track.h"

#include <vector>

// Stores flags.
// author: C. Bernet, B. Hegner

//forward declaration of Track container
class TrackCollection;

namespace albers {
  class Registry;
}

class TrackHandle {

  friend class TrackCollection;

public:

TrackHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  TrackHandle() = default;

  const float& Chi2() const;
  const unsigned& Ndf() const;
  const unsigned& Bits() const;

  void setChi2(float value);
  void setNdf(unsigned value);
  void setBits(unsigned value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const TrackHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const TrackHandle& p1,
			 const TrackHandle& p2 );

private:
  TrackHandle(int index, int containerID,  std::vector<Track>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<Track>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
