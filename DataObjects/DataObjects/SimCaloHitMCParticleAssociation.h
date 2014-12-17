#ifndef SimCaloHitMCParticleAssociation_H 
#define SimCaloHitMCParticleAssociation_H

// Fraction of the particle energy used in the hit.
// author: C. Bernet, B. Hegner

#include "DataObjects/SimCaloHitHandle.h"
#include "DataObjects/MCParticleHandle.h"


class SimCaloHitMCParticleAssociation {
public:
  SimCaloHitHandle Hit; //The hit. 
  MCParticleHandle Particle; //The particle. 
  float Fraction; //Fraction of the particle energy used in the hit. 

};

#endif