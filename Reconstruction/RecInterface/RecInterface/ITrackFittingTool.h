#ifndef RECINTERFACE_ITRACKFITTINGTOOL_H
#define RECINTERFACE_ITRACKFITTINGTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include <map>

namespace edm4hep {
class TrackCollection;
class TrackerHitCollection;
}

/** \brief High level interface for a track fitter
 *
 * Turn a collection of hits and the map describing the track seeds into tracks.
 */
class ITrackFittingTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ITrackFittingTool, 1, 0);

  /// Create tracks out of `theHits`, using the seeding information in `seedmap`
  virtual edm4hep::TrackCollection
  fitTracks(const edm4hep::TrackerHitCollection* theHits, std::multimap<unsigned int, unsigned int> seedmap) = 0;
};

#endif /* RECINTERFACE_ITRACKFITTINGTOOL_H */
