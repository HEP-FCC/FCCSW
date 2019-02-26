#ifndef TRICKTRACK_CMCELL_H 
#define TRICKTRACK_CMCELL_H 

#include <cmath>
#include <array>
#include <iostream>
#include <functional>

#include "tricktrack/HitDoublets.h"

namespace tricktrack {
  template <typename Hit>
  class CMCell;
}

namespace tricktrack {

  template <typename Hit>
  using TripletFilter = std::function<bool (const CMCell<Hit> &, const CMCell <Hit>&)>;



/** @class CMCellStatus 
 *
 * Internal property of a Cell acted on during evolution.
 *
 */
class CMCellStatus {

public:
  
  /// getter for the State
  unsigned char getCAState() const {
    return theCAState;
  }
  
  /// if there is at least one left neighbor with the same state (friend), the state has to be increased by 1.
  void updateState() {
    theCAState += hasSameStateNeighbors;
  }
  
  bool isRootCell(const unsigned int minimumCAState) const {
    return (theCAState >= minimumCAState);
  }
  
 public:
  unsigned char theCAState=0;
  unsigned char hasSameStateNeighbors=0;
  
};


/** @class CMCell
 *
 * Links of the chain. The cell is the smallest unit of the algorithm and 
 * carries indices referring to the doublet and input hits. Tracklets are built
 * by updating its state.
 * */
template <typename Hit>
class CMCell {
public:
  using CMntuple = std::vector<unsigned int>;
  using CMntuplet = std::vector<unsigned int>;
  using CMColl = std::vector<CMCell<Hit>>;
  using CAStatusColl = std::vector<CMCellStatus>;
  
  
  CMCell(const HitDoublets<Hit>* doublets, int doubletId, const int innerHitId, const int outerHitId) :
    theDoublets(doublets), theDoubletId(doubletId)
    ,theInnerR(doublets->rv(doubletId, HitDoublets<Hit>::inner)) 
    ,theInnerZ(doublets->z(doubletId, HitDoublets<Hit>::inner))
  {}

   
  
  Hit const & getInnerHit() const {
    return theDoublets->hit(theDoubletId, HitDoublets<Hit>::inner);
  }
  
  Hit const & getOuterHit() const {
    return theDoublets->hit(theDoubletId, HitDoublets<Hit>::outer);
  }
  
  
  float getInnerX() const {
    return theDoublets->x(theDoubletId, HitDoublets<Hit>::inner);
  }
  
  float getOuterX() const {
    return theDoublets->x(theDoubletId, HitDoublets<Hit>::outer);
  }
  
  float getInnerY() const {
    return theDoublets->y(theDoubletId, HitDoublets<Hit>::inner);
  }
  
  float getOuterY() const {
    return theDoublets->y(theDoubletId, HitDoublets<Hit>::outer);
  }
  
  float getInnerZ() const {
    return theInnerZ;
  }
  
  float getOuterZ() const {
    return theDoublets->z(theDoubletId, HitDoublets<Hit>::outer);
  }
  
  float getInnerR() const {
    return theInnerR;
  }
  
  float getOuterR() const {
    return theDoublets->rv(theDoubletId, HitDoublets<Hit>::outer);
  }
  
  float getInnerPhi() const {
    return theDoublets->phi(theDoubletId, HitDoublets<Hit>::inner);
  }
  
  float getOuterPhi() const {
    return theDoublets->phi(theDoubletId, HitDoublets<Hit>::outer);
  }
  
  /// local action undertaken during CM evolution:
  /// the state is increased if the cell has neighbors with the same state
  void evolve(unsigned int me, CAStatusColl& allStatus) {
    
    allStatus[me].hasSameStateNeighbors = 0;
    auto mystate = allStatus[me].theCAState;
    
    for (auto oc : theOuterNeighbors ) {
      
      if (allStatus[oc].getCAState() == mystate) {
	
	allStatus[me].hasSameStateNeighbors = 1;
	
	break;
      }
    }
    
  }
  
  void checkAlignmentAndAct(CMColl& allCells, CMntuple & innerCells, std::vector<CMCell::CMntuplet> * foundTriplets, TripletFilter<Hit> theFilter) {
    int ncells = innerCells.size();
    int constexpr VSIZE = 16;
    //int ok[VSIZE];
    float r1[VSIZE];
    float z1[VSIZE];
    auto ro = getOuterR();
    auto zo = getOuterZ();
    unsigned int cellId = this - &allCells.front();
    auto loop = [&](int i, int vs) {
      for (int j=0;j<vs; ++j) {
	auto koc = innerCells[i+j];
	auto & oc =  allCells[koc];
	r1[j] = oc.getInnerR();
	z1[j] = oc.getInnerZ();
      }
      // this vectorize!
      //for (int j=0;j<vs; ++j) ok[j] = areAlignedRZ(r1[j], z1[j], ro, zo, ptmin, thetaCut);
      for (int j=0;j<vs; ++j) {
	auto koc = innerCells[i+j];
	auto & oc =  allCells[koc]; 
	 if (theFilter(oc, *this)) {
	  if (foundTriplets) 
      foundTriplets->emplace_back(CMCell::CMntuplet{koc,cellId});
	  else {
	    oc.tagAsOuterNeighbor(cellId);
	  }
	}
      }
    };
    auto lim = VSIZE*(ncells/VSIZE);
    for (int i=0; i<lim; i+=VSIZE) loop(i, VSIZE); 
    loop(lim, ncells-lim);
    
  }

  
  void checkAlignmentAndTag(CMColl& allCells, CMntuple & innerCells, TripletFilter<Hit> theFilter) {
    checkAlignmentAndAct(allCells, innerCells, nullptr, theFilter);
    
  }
  
  void checkAlignmentAndPushTriplet(CMColl& allCells, CMntuple & innerCells, std::vector<CMCell::CMntuplet>& foundTriplets,
				    TripletFilter<Hit> theFilter) {
    checkAlignmentAndAct(allCells, innerCells, &foundTriplets, theFilter);
  }
  
  
  void tagAsOuterNeighbor(unsigned int otherCell)
  {
    theOuterNeighbors.push_back(otherCell);
  }
  
  
  
  
  // trying to free the track building process from hardcoded layers, leaving the visit of the graph
  // based on the neighborhood connections between cells.
  
  void findNtuplets(CMColl& allCells, std::vector<CMntuplet>& foundNtuplets, CMntuplet& tmpNtuplet, const unsigned int minHitsPerNtuplet) const {
    
    // the building process for a track ends if:
    // it has no outer neighbor
    // it has no compatible neighbor
    // the ntuplets is then saved if the number of hits it contains is greater than a threshold
    
    if (tmpNtuplet.size() == minHitsPerNtuplet - 1)
      {
	foundNtuplets.push_back(tmpNtuplet);
      }
    else
      {
	unsigned int numberOfOuterNeighbors = theOuterNeighbors.size();
	for (unsigned int i = 0; i < numberOfOuterNeighbors; ++i) {
	  tmpNtuplet.push_back((theOuterNeighbors[i]));
	  allCells[theOuterNeighbors[i]].findNtuplets(allCells,foundNtuplets, tmpNtuplet, minHitsPerNtuplet);
	  tmpNtuplet.pop_back();
	}
      }
    
  }
  
  
private:
  
  CMntuple theOuterNeighbors;
  
  /// the doublet container for this layer
  const HitDoublets<Hit>* theDoublets;  
  /// the index of the cell doublet in the doublet container
  const int theDoubletId;
  
  /// cache of the z-coordinate of the doublet on the inner layer
  const float theInnerR;
  /// cache of the r-coordinate of the doublet on the inner layer
  const float theInnerZ;
  
};

} // namespace tricktrack

#endif /* TRICKTRACK_CMCELL_H */
