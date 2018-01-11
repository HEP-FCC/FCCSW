#ifndef RECTRACKER_TRACKINGUTILS_H
#define RECTRACKER_TRACKINGUTILS_H

#include <vector>

namespace dd4hep {
  namespace DDSegmentation {
    class BitField64;
  }
}

namespace fcc {
 class TrackHitCollection;
 class TrackHit;
}

namespace rec {
/// fill vector with hits ordered according to the CellIdOrder struct
void sortTrackHits(const fcc::TrackHitCollection* unsortedHits, std::vector<fcc::TrackHit>& sortedHits, dd4hep::DDSegmentation::BitField64* decoder);

}
#endif
