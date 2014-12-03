#ifndef VertexTrackAssociation_H 
#define VertexTrackAssociation_H

// Weight of the track in the vertex.
// author: C. Bernet, B. Hegner

#include "DataObjects/TrackHandle.h"
#include "DataObjects/VertexHandle.h"


class VertexTrackAssociation {
public:
  TrackHandle Track; //The track. 
  VertexHandle Vertex; //The vertex. 
  float Weight; //Weight of the track in the vertex. 

};

#endif