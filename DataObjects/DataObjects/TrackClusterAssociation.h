#ifndef TrackClusterAssociation_H 
#define TrackClusterAssociation_H

// The cluster.
// author: C. Bernet, B. Hegner

#include "DataObjects/TrackHandle.h"
#include "DataObjects/TrackClusterHandle.h"


class TrackClusterAssociation {
public:
  TrackHandle Track; //The track. 
  TrackClusterHandle Cluster; //The cluster. 

};

#endif