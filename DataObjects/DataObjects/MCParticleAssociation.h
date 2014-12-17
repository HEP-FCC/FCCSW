#ifndef MCParticleAssociation_H 
#define MCParticleAssociation_H

// The daughter.
// author: C. Bernet, B. Hegner

#include "DataObjects/MCParticleHandle.h"
#include "DataObjects/MCParticleHandle.h"


class MCParticleAssociation {
public:
  MCParticleHandle Mother; //The mother. 
  MCParticleHandle Daughter; //The daughter. 

};

#endif