#ifndef MCParticleAssociationHANDLE_H
#define MCParticleAssociationHANDLE_H
#include "DataObjects/MCParticleAssociation.h"
#include "DataObjects/MCParticleHandle.h"
#include "DataObjects/MCParticleHandle.h"

#include <vector>

// The daughter.
// author: C. Bernet, B. Hegner

//forward declaration of MCParticleAssociation container
class MCParticleAssociationCollection;

namespace albers {
  class Registry;
}

class MCParticleAssociationHandle {

  friend class MCParticleAssociationCollection;

public:

MCParticleAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  MCParticleAssociationHandle() = default;

  const MCParticleHandle& Mother() const;
  const MCParticleHandle& Daughter() const;

  void setMother(MCParticleHandle value);
  void setDaughter(MCParticleHandle value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const MCParticleAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const MCParticleAssociationHandle& p1,
			 const MCParticleAssociationHandle& p2 );

private:
  MCParticleAssociationHandle(int index, int containerID,  std::vector<MCParticleAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<MCParticleAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
