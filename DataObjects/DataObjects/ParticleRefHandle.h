#ifndef ParticleRefHANDLE_H
#define ParticleRefHANDLE_H
#include "DataObjects/ParticleRef.h"
#include "DataObjects/ParticleHandle.h"

#include <vector>

// n
// author: C. Bernet, B. Hegner

//forward declaration of ParticleRef container
class ParticleRefCollection;

namespace albers {
  class Registry;
}

class ParticleRefHandle {

  friend ParticleRefCollection;

public:

ParticleRefHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  ParticleRefHandle() = default;

  const ParticleHandle& Particle() const;

  void setParticle(ParticleHandle value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  ParticleRefHandle(int index, int containerID,  std::vector<ParticleRef>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<ParticleRef>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
