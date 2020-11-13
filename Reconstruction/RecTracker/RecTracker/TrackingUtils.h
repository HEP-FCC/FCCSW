#ifndef RECTRACKER_TRACKINGUTILS_H
#define RECTRACKER_TRACKINGUTILS_H

#include <vector>

namespace dd4hep {
  namespace DDSegmentation {
    class BitFieldCoder;
  }
}

namespace edm4hep {
 class SimTrackerHitCollection;
 class SimTrackerHit;
}

namespace rec {
/// fill vector with hits ordered according to the CellIdOrder struct
void sortTrackHits(const edm4hep::SimTrackerHitCollection* unsortedHits, std::vector<edm4hep::SimTrackerHit>& sortedHits, const dd4hep::DDSegmentation::BitFieldCoder* decoder);

}
#endif
