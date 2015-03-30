///////////////////////////////////////////////////////////////////
// TrackParameters.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERS_TRACKPARAMETERS_H
#define TRKPARAMETERS_TRACKPARAMETERS_H


#include "TrkParametersBase/ParametersT.h"
#include "TrkParametersBase/Charged.h"
#include "RecoGeometry/PlaneSurface.h"
#include "RecoGeometry/CylinderSurface.h"
#include "RecoGeometry/DiscSurface.h"

namespace Trk {

    typedef ParametersBase<5, Charged>                       TrackParameters;
    typedef ParametersT<5,Charged,Reco::PlaneSurface>        TrackPlane;
    typedef ParametersT<5,Charged,Reco::CylinderSurface>     TrackCylinder;
    typedef ParametersT<5,Charged,Reco::DiscSurface>          TrackDisc;
    
}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const Trk::TrackParameters& pars);

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Trk::TrackParameters& pars);

#endif
