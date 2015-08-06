#include "TrkNeutralParameters/NeutralParameters.h"

namespace Trk {

 template class ParametersBase<5,Neutral>;
 template class ParametersT<5,Neutral,Reco::PlaneSurface>;
 template class ParametersT<5,Neutral,Reco::CylinderSurface>;
 template class ParametersT<5,Neutral,Reco::DiscSurface>;

}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const Trk::NeutralParameters& pars)
{ return pars.dump(sl); }

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Trk::NeutralParameters& pars)
{ return pars.dump(sl); }