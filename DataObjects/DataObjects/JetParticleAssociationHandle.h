#ifndef JetParticleAssociationHANDLE_H
#define JetParticleAssociationHANDLE_H
#include "DataObjects/JetParticleAssociation.h"
#include "DataObjects/JetHandle.h"
#include "DataObjects/ParticleHandle.h"

#include <vector>

// Reference to the particle
// author: C. Bernet, B. Hegner

//forward declaration of JetParticleAssociation container
class JetParticleAssociationCollection;

namespace albers {
  class Registry;
}

class JetParticleAssociationHandle {

  friend class JetParticleAssociationCollection;

public:

  JetParticleAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  JetParticleAssociationHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const JetParticleAssociation& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  JetParticleAssociation& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const JetParticleAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const JetParticleAssociationHandle& p1,
			 const JetParticleAssociationHandle& p2 );

private:
  JetParticleAssociationHandle(int index, int containerID,  std::vector<JetParticleAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<JetParticleAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
