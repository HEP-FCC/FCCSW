#ifndef ParticleMCAssociationHANDLE_H
#define ParticleMCAssociationHANDLE_H
#include "DataObjects/ParticleMCAssociation.h"
#include "DataObjects/ParticleHandle.h"
#include "DataObjects/MCParticleHandle.h"

#include <vector>

// Reference to the Monte-Carlo particle
// author: C. Bernet, B. Hegner

//forward declaration of ParticleMCAssociation container
class ParticleMCAssociationCollection;

namespace albers {
  class Registry;
}

class ParticleMCAssociationHandle {

  friend class ParticleMCAssociationCollection;

public:

ParticleMCAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  ParticleMCAssociationHandle() = default;

  const ParticleHandle& Rec() const;
  const MCParticleHandle& Sim() const;

  void setRec(ParticleHandle value);
  void setSim(MCParticleHandle value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const ParticleMCAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const ParticleMCAssociationHandle& p1,
			 const ParticleMCAssociationHandle& p2 );

private:
  ParticleMCAssociationHandle(int index, int containerID,  std::vector<ParticleMCAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<ParticleMCAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
