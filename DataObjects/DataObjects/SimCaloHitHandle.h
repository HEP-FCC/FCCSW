#ifndef SimCaloHitHANDLE_H
#define SimCaloHitHANDLE_H
#include "DataObjects/SimCaloHit.h"
#include "DataObjects/BareHit.h"

#include <vector>

// contains basic hit information
// author: C. Bernet, B. Hegner

//forward declaration of SimCaloHit container
class SimCaloHitCollection;

namespace albers {
  class Registry;
}

class SimCaloHitHandle {

  friend class SimCaloHitCollection;

public:

SimCaloHitHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  SimCaloHitHandle() = default;

  const BareHit& Core() const;

  void setCore(BareHit value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const SimCaloHitHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const SimCaloHitHandle& p1,
			 const SimCaloHitHandle& p2 );

private:
  SimCaloHitHandle(int index, int containerID,  std::vector<SimCaloHit>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<SimCaloHit>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
