#ifndef JetParticleHANDLE_H
#define JetParticleHANDLE_H
#include "DataObjects/JetParticle.h"
#include "DataObjects/JetHandle.h"
#include "DataObjects/ParticleHandle.h"

#include <vector>

// 
// author: C. Bernet, B. Hegner

//forward declaration of JetParticle container
class JetParticleCollection;

namespace albers {
  class Registry;
}

class JetParticleHandle {

  friend JetParticleCollection;

public:

JetParticleHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  JetParticleHandle() = default;

  const JetHandle& Jet() const;
  const ParticleHandle& Particle() const;

  void setJet(JetHandle value);
  void setParticle(ParticleHandle value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  JetParticleHandle(int index, int containerID,  std::vector<JetParticle>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<JetParticle>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
