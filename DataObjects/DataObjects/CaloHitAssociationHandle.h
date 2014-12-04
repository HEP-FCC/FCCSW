#ifndef CaloHitAssociationHANDLE_H
#define CaloHitAssociationHANDLE_H
#include "DataObjects/CaloHitAssociation.h"
#include "DataObjects/CaloHitHandle.h"
#include "DataObjects/SimCaloHitHandle.h"

#include <vector>

// The simulated hit.
// author: C. Bernet, B. Hegner

//forward declaration of CaloHitAssociation container
class CaloHitAssociationCollection;

namespace albers {
  class Registry;
}

class CaloHitAssociationHandle {

  friend class CaloHitAssociationCollection;

public:

  CaloHitAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  CaloHitAssociationHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const CaloHitAssociation& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  CaloHitAssociation& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const CaloHitAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const CaloHitAssociationHandle& p1,
			 const CaloHitAssociationHandle& p2 );

private:
  CaloHitAssociationHandle(int index, int containerID,  std::vector<CaloHitAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<CaloHitAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
