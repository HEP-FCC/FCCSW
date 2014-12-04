#ifndef TrackClusterHitsAssociation_H 
#define TrackClusterHitsAssociation_H

// hit handle
// author: C. Bernet, B. Hegner

#include "DataObjects/TrackClusterHandle.h"
#include "DataObjects/TrackHitHandle.h"


class TrackClusterHitsAssociation {
public:
  TrackClusterHandle Cluster; //cluster handle 
  TrackHitHandle Hit; //hit handle 

};

#endif