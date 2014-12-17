#ifndef JetParticleAssociation_H 
#define JetParticleAssociation_H

// Reference to the particle
// author: C. Bernet, B. Hegner

#include "DataObjects/JetHandle.h"
#include "DataObjects/ParticleHandle.h"


class JetParticleAssociation {
public:
  JetHandle Jet; //Reference to the jet 
  ParticleHandle Particle; //Reference to the particle 

};

#endif