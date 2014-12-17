#ifndef Vertex_H 
#define Vertex_H

// Stores flags.
// author: C. Bernet, B. Hegner

#include "DataObjects/Point.h"


class Vertex {
public:
  float Chi2; //chi2 returned by the vertex fit 
  unsigned Ndf; //Number of degrees of freedom of the vertex fit 
  Point Position; //Vertex position in cm 
  unsigned Bits; //Stores flags. 

};

#endif