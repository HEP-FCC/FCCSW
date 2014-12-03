#ifndef TrackState_H 
#define TrackState_H

// Longitudinal impact parameter
// author: C. Bernet, B. Hegner



class TrackState {
public:
  float Location; //Location on the track. (Radius?) 
  float Omega; //Track curvature in cm. 
  float D0; //Transverse impact parameter 
  float Z0; //Longitudinal impact parameter 

};

#endif