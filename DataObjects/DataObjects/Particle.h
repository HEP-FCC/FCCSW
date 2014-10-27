#ifndef Particle_H 
#define Particle_H

// Status code
// author: C. Bernet, B. Hegner

#include "LorentzVector.h"


class Particle {
public:
  LorentzVector P4; //Four momentum. 
  int ID; //Particle type identification code 
  int Status; //Status code 

};

#endif