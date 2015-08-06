///////////////////////////////////////////////////////////////////
// NeutralParameters.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKNEUTRALPARAMETERS_NEUTRALPARAMETERS_H
#define TRKNEUTRALPARAMETERS_NEUTRALPARAMETERS_H

#include "TrkParametersBase/ParametersT.h"
#include "TrkParametersBase/Neutral.h"
#include "RecoGeometry/PlaneSurface.h"
#include "RecoGeometry/CylinderSurface.h"
#include "RecoGeometry/DiscSurface.h"

namespace Trk {

    typedef ParametersBase<5, Neutral>                              NeutralParameters;
    typedef ParametersT<5,Neutral,Reco::PlaneSurface>               NeutralPlane;
    typedef ParametersT<5,Neutral,Reco::CylinderSurface>            NeutralCylinder;
    typedef ParametersT<5,Neutral,Reco::DiscSurface>                NeutralDisc;

}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const Trk::NeutralParameters& pars);

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Trk::NeutralParameters& pars);

#endif // TRKNEUTRALPARAMETERS_NEUTRALPARAMETERS_H
