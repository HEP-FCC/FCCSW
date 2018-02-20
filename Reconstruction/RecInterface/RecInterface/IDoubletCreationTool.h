#ifndef RECINTERFACE_IDOUBLETCREATIONTOOL_H
#define RECINTERFACE_IDOUBLETCREATIONTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include <map>

// TrickTrack headers
#include "tricktrack/HitChainMaker.h"
#include "tricktrack/HitDoublets.h"
#include "tricktrack/SpacePoint.h"
#include "tricktrack/TTPoint.h"
//using Hit = tricktrack::SpacePoint<size_t>; 
using Hit = tricktrack::TTPoint; 

namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}


class IDoubletCreationTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IDoubletCreationTool, 1, 0);

  virtual tricktrack::HitDoublets<Hit>*   findDoublets(std::vector<Hit> theInnerHits, std::vector<Hit> theOuterHits) = 0;
};

#endif /* RECINTERFACE_IDOUBLETCREATIONTOOL_H */
