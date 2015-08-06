///////////////////////////////////////////////////////////////////
// ChargeDefinition.cxx (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkParametersBase/ChargeDefinition.h"

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::ChargeDefinition& chdef)
{
  chdef.dump(sl);
  return sl;
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::ChargeDefinition& chdef)
{
  chdef.dump(sl);
  return sl;
}

