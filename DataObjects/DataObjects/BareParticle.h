#ifndef BareParticle_H 
#define BareParticle_H

#include "DataObjects/Point.h"
#include "DataObjects/LorentzVector.h"


class BareParticle {
public:
  unsigned Status;
  unsigned Bits;
  int Type;
  Point Vertex;
  LorentzVector P4;

};

#endif