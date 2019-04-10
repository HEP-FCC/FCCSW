#ifndef RECINTERFACE_ITRACKEXTRAPOLATIONTOOL_H
#define RECINTERFACE_ITRACKEXTRAPOLATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "datamodel/PositionedTrackHit.h"
#include "datamodel/TrackState.h"

/** @class ITrackExtrapolationTool RecInterface/RecInterface/ITrackExtrapolationTool.h
 * ITrackExtrapolationTool.h
 *
 *  Abstract interface fo track extrapolation tool
 */

class ITrackExtrapolationTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ITrackExtrapolationTool, 1, 0);

  virtual std::vector<fcc::TrackState> extrapolate(fcc::TrackState theTrackState) = 0;
};

#endif /* RECINTERFACE_IEXTRAPOLATIONTOOL_H */
