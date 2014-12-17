#ifndef CaloHitAssociation_H 
#define CaloHitAssociation_H

// The simulated hit.
// author: C. Bernet, B. Hegner

#include "DataObjects/CaloHitHandle.h"
#include "DataObjects/SimCaloHitHandle.h"


class CaloHitAssociation {
public:
  CaloHitHandle Rec; //The reconstruted hit. 
  SimCaloHitHandle Sim; //The simulated hit. 

};

#endif