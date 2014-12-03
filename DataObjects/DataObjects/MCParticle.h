#ifndef MCParticle_H 
#define MCParticle_H

// Position of the decay.
// author: C. Bernet, B. Hegner

#include "DataObjects/BareParticle.h"
#include "DataObjects/Point.h"


class MCParticle {
public:
  BareParticle Core; //Basic particle information. 
  Point Endvertex; //Position of the decay. 

};

#endif