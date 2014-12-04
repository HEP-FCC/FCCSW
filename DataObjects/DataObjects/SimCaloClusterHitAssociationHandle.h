#ifndef SimCaloClusterHitAssociationHANDLE_H
#define SimCaloClusterHitAssociationHANDLE_H
#include "DataObjects/SimCaloClusterHitAssociation.h"
#include "DataObjects/SimCaloClusterHandle.h"
#include "DataObjects/SimCaloHitHandle.h"

#include <vector>

// The hit
// author: C. Bernet, B. Hegner

//forward declaration of SimCaloClusterHitAssociation container
class SimCaloClusterHitAssociationCollection;

namespace albers {
  class Registry;
}

class SimCaloClusterHitAssociationHandle {

  friend class SimCaloClusterHitAssociationCollection;

public:

  SimCaloClusterHitAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  SimCaloClusterHitAssociationHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const SimCaloClusterHitAssociation& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  SimCaloClusterHitAssociation& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const SimCaloClusterHitAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const SimCaloClusterHitAssociationHandle& p1,
			 const SimCaloClusterHitAssociationHandle& p2 );

private:
  SimCaloClusterHitAssociationHandle(int index, int containerID,  std::vector<SimCaloClusterHitAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<SimCaloClusterHitAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
