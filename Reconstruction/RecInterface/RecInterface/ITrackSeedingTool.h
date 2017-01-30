#ifndef RECINTERFACE_ITRACKSEEDINGTOOL_H
#define RECINTERFACE_ITRACKSEEDINGTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}

namespace rec {
class TrackSeed;
}

class ITrackSeedingTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ITrackSeedingTool, 1, 0);

  virtual rec::TrackSeed findSeeds(fcc::PositionedTrackHitCollection theHits) = 0;
};

#endif /* RECINTERFACE_ITRACKSEEDINGTOOL_H */
