#ifndef ParticleClusterAssociationHANDLE_H
#define ParticleClusterAssociationHANDLE_H
#include "DataObjects/ParticleClusterAssociation.h"
#include "DataObjects/ParticleHandle.h"
#include "DataObjects/CaloClusterHandle.h"

#include <vector>

// The cluster
// author: C. Bernet, B. Hegner

//forward declaration of ParticleClusterAssociation container
class ParticleClusterAssociationCollection;

namespace albers {
  class Registry;
}

class ParticleClusterAssociationHandle {

  friend class ParticleClusterAssociationCollection;

public:

ParticleClusterAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  ParticleClusterAssociationHandle() = default;

  const ParticleHandle& Particle() const;
  const CaloClusterHandle& Cluster() const;

  void setParticle(ParticleHandle value);
  void setCluster(CaloClusterHandle value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const ParticleClusterAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const ParticleClusterAssociationHandle& p1,
			 const ParticleClusterAssociationHandle& p2 );

private:
  ParticleClusterAssociationHandle(int index, int containerID,  std::vector<ParticleClusterAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<ParticleClusterAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
