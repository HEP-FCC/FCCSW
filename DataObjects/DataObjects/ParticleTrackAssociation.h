#ifndef ParticleTrackAssociation_H 
#define ParticleTrackAssociation_H

// The track
// author: C. Bernet, B. Hegner

#include "DataObjects/ParticleHandle.h"
#include "DataObjects/TrackHandle.h"


class ParticleTrackAssociation {
public:
  ParticleHandle Particle; //The particle 
  TrackHandle Track; //The track 

};

#endif