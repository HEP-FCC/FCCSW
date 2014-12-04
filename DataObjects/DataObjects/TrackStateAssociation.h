#ifndef TrackStateAssociation_H 
#define TrackStateAssociation_H

// The track state.
// author: C. Bernet, B. Hegner

#include "DataObjects/TrackHandle.h"
#include "DataObjects/TrackStateHandle.h"


class TrackStateAssociation {
public:
  TrackHandle Track; //The track. 
  TrackStateHandle State; //The track state. 

};

#endif