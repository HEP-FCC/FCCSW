#ifndef Track_H 
#define Track_H

// Stores flags.
// author: C. Bernet, B. Hegner



class Track {
public:
  float Chi2; //chi2 returned by the track fit 
  unsigned Ndf; //Number of degrees of freedom of the track fit 
  unsigned Bits; //Stores flags. 

};

#endif