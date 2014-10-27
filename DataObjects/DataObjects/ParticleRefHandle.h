#ifndef ParticleRefHANDLE_H
#define ParticleRefHANDLE_H
#include "ParticleRef.h"
#include "ParticleHandle.h"

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

  // COLIN: too painful to call each setter one by one, and unsafe. remove setters and use a parameter list in the constructor? or an init function2222
  const ParticleHandle& Particle() const;

  void setParticle(ParticleHandle value);


  // COLIN: I'd make that a true const method, and would set m_container in prepareAFterRead. What if the user doesn't call that?
  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  ParticleRefHandle(int index, int containerID,  std::vector<ParticleRef>* container);
  int m_index;
  int m_containerID;
  // COLIN: after reading, the transient m_container address must be taken from the registry using the persistent m_containerID and set. This seems to happen in isAvailable... why not in prepareAfterRead?
  mutable std::vector<ParticleRef>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();

};

#endif
