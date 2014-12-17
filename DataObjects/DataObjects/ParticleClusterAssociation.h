#ifndef ParticleClusterAssociation_H 
#define ParticleClusterAssociation_H

// The cluster
// author: C. Bernet, B. Hegner

#include "DataObjects/ParticleHandle.h"
#include "DataObjects/CaloClusterHandle.h"


class ParticleClusterAssociation {
public:
  ParticleHandle Particle; //The particle 
  CaloClusterHandle Cluster; //The cluster 

};

#endif