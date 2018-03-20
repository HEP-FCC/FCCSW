#ifndef RECINTERFACE_ITRACKFITTINGTOOL_H
#define RECINTERFACE_ITRACKFITTINGTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include <map>

namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
class TrackState;
}


class ITrackFittingTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ITrackFittingTool, 1, 0);

  virtual fcc::TrackState fitTrack(const fcc::PositionedTrackHitCollection* theHits, std::vector<unsigned int> hitIndices) = 0;
};

#endif /* RECINTERFACE_ITRACKFITTINGTOOL_H */
