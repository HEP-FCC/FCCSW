#ifndef SimCaloHitMCParticleAssociationHANDLE_H
#define SimCaloHitMCParticleAssociationHANDLE_H
#include "DataObjects/SimCaloHitMCParticleAssociation.h"
#include "DataObjects/SimCaloHitHandle.h"
#include "DataObjects/MCParticleHandle.h"

#include <vector>

// Fraction of the particle energy used in the hit.
// author: C. Bernet, B. Hegner

//forward declaration of SimCaloHitMCParticleAssociation container
class SimCaloHitMCParticleAssociationCollection;

namespace albers {
  class Registry;
}

class SimCaloHitMCParticleAssociationHandle {

  friend class SimCaloHitMCParticleAssociationCollection;

public:

  SimCaloHitMCParticleAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  SimCaloHitMCParticleAssociationHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const SimCaloHitMCParticleAssociation& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  SimCaloHitMCParticleAssociation& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const SimCaloHitMCParticleAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const SimCaloHitMCParticleAssociationHandle& p1,
			 const SimCaloHitMCParticleAssociationHandle& p2 );

private:
  SimCaloHitMCParticleAssociationHandle(int index, int containerID,  std::vector<SimCaloHitMCParticleAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<SimCaloHitMCParticleAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
