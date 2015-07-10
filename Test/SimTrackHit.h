//
//  SimTrackHit.h
//  
//
//  Created by Julia Hrdinka on 13/05/15.
//
//

#ifndef _SimTrackHit_h
#define _SimTrackHit_h

#include "DataObjects/Point.h"
#inlcude "/SurfaceData.h"


class SimTrackHit {

    Point entry;
    Point exit;
    float dE;
    unsigned long ModuleID;
    SurfaceData* surface;
};


#endif
