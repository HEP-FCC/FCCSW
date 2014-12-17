#ifndef GenJetParticleAssociationHANDLE_H
#define GenJetParticleAssociationHANDLE_H
#include "DataObjects/GenJetParticleAssociation.h"
#include "DataObjects/GenJetHandle.h"
#include "DataObjects/MCParticleHandle.h"

#include <vector>

// Reference to the mc particle
// author: C. Bernet, B. Hegner

//forward declaration of GenJetParticleAssociation container
class GenJetParticleAssociationCollection;

namespace albers {
  class Registry;
}

class GenJetParticleAssociationHandle {

  friend class GenJetParticleAssociationCollection;

public:

  GenJetParticleAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  GenJetParticleAssociationHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const GenJetParticleAssociation& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  GenJetParticleAssociation& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const GenJetParticleAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const GenJetParticleAssociationHandle& p1,
			 const GenJetParticleAssociationHandle& p2 );

private:
  GenJetParticleAssociationHandle(int index, int containerID,  std::vector<GenJetParticleAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<GenJetParticleAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
