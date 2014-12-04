#ifndef ParticleHANDLE_H
#define ParticleHANDLE_H
#include "DataObjects/Particle.h"
#include "DataObjects/BareParticle.h"

#include <vector>

// Contains basic particle information.
// author: C. Bernet, B. Hegner

//forward declaration of Particle container
class ParticleCollection;

namespace albers {
  class Registry;
}

class ParticleHandle {

  friend class ParticleCollection;

public:

ParticleHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  ParticleHandle() = default;

  const BareParticle& Core() const;

  void setCore(BareParticle value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const ParticleHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const ParticleHandle& p1,
			 const ParticleHandle& p2 );

private:
  ParticleHandle(int index, int containerID,  std::vector<Particle>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<Particle>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
