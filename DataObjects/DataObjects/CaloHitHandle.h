#ifndef CaloHitHANDLE_H
#define CaloHitHANDLE_H
#include "DataObjects/CaloHit.h"
#include "DataObjects/BareHit.h"

#include <vector>

// contains basic hit information
// author: C. Bernet, B. Hegner

//forward declaration of CaloHit container
class CaloHitCollection;

namespace albers {
  class Registry;
}

class CaloHitHandle {

  friend class CaloHitCollection;

public:

  CaloHitHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  CaloHitHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const CaloHit& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  CaloHit& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const CaloHitHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const CaloHitHandle& p1,
			 const CaloHitHandle& p2 );

private:
  CaloHitHandle(int index, int containerID,  std::vector<CaloHit>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<CaloHit>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
