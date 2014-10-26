#ifndef JetHANDLE_H
#define JetHANDLE_H
#include "DataObjects/Jet.h"
#include "DataObjects/LorentzVector.h"
#include <vector>
#include "ParticleHandle.h"

#include <vector>

// 
// author: C. Bernet, B. Hegner

//forward declaration of Jet container
class JetCollection;

namespace albers {
  class Registry;
}

class JetHandle {

  friend class JetCollection;

public:

JetHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  JetHandle() = default;

  const LorentzVector& P4() const;

  void setP4(LorentzVector value);

  void addparticles(ParticleHandle&);
  std::vector<ParticleHandle>::const_iterator particles_begin() const;
  std::vector<ParticleHandle>::const_iterator particles_end() const;


  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  JetHandle(int index, int containerID,  std::vector<Jet>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<Jet>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  std::vector<ParticleHandle>* m_particles; //! transient 


};

#endif
