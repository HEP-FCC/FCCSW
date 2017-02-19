
#ifndef KDDOUBLETS_H
#define KDDOUBLETS_H

#include "FKDPoint.h"
#include "datamodel/PositionedTrackHitCollection.h"

#include <vector>
#include <array>
#include <cmath>


class HitDoublets {
public:
  using Hit=fcc::PositionedTrackHit;
  enum layer { inner=0, outer=1};

  HitDoublets(std::vector<Hit>& innerHits, std::vector<Hit>& outerHits): layers{{innerHits, outerHits}}{}


  Hit const & hit(int i, layer l) const { return layers[l][indices[2*i + l]];}
  float x(int i, layer l) const { return layers[l][indices[2*i + l]].position().x;}
  float y(int i, layer l) const { return layers[l][indices[2*i + l]].position().y;}
  float z(int i, layer l) const { return layers[l][indices[2*i + l]].position().z;}
  float r(int i, layer l) const { return std::sqrt(x(i,l)*x(i,l) + y(i,l) *y(i,l)  );}
  float phi(int i, layer l) const { return std::atan2(y(i,l), x(i,l));}
  void add(int innerHit, int outerHit) { indices.push_back(innerHit); indices.push_back(outerHit); } 
  size_t size() const {return indices.size()/2;}
  int innerHitId(int i) const {return indices[2*i];}
  int outerHitId(int i) const {return indices[2*i+1];}
  std::array<std::vector<Hit>,2> layers;  
  Hit testHit_inner;
  Hit testHit_outer;
  Hit testHit;
  std::vector<int> indices;

};



#endif
