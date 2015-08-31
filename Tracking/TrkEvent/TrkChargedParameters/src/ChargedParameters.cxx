///////////////////////////////////////////////////////////////////
// TrackParameters.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkChargedParameters/ChargedParameters.h"

namespace Trk {

 template class ParametersBase<5,Charged>;
 template class ParametersT<5,Charged,Reco::PlaneSurface>;
 template class ParametersT<5,Charged,Reco::CylinderSurface>;
 template class ParametersT<5,Charged,Reco::DiscSurface>;

}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const Trk::TrackParameters& pars)
{ return pars.dump(sl); }

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Trk::TrackParameters& pars)
{ return pars.dump(sl); }