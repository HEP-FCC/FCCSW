
#include "RecTracker/TrackingUtils.h"

#include "DD4hep/Detector.h"
#include "DDSegmentation/BitFieldCoder.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"


namespace rec {

// For use with standard sorting algorithms
// compares the system fields of the hits first, then the layer field
struct CellIdOrder {
  CellIdOrder(const dd4hep::DDSegmentation::BitFieldCoder* dec) : m_decoder(dec) {}

  bool operator()(const fcc::TrackHit& h1, const fcc::TrackHit& h2) {
    dd4hep::DDSegmentation::CellID cID = h1.cellId();
    int system1 = m_decoder->get(cID, "system");
    int layer1 = m_decoder->get(cID, "layer");
    dd4hep::DDSegmentation::CellID cID2 = h2.cellId();
    int system2 = m_decoder->get(cID2, "system");
    int layer2 = m_decoder->get(cID2, "layer");
    if (system1 < system2) return true;
    if (system2 < system1) return false;
    return (layer1 < layer2);
  }

private:
  const dd4hep::DDSegmentation::BitFieldCoder* m_decoder;
};

/// fill vector with hits ordered according to the CellIdOrder struct
void sortTrackHits(const fcc::TrackHitCollection* unsortedHits, std::vector<fcc::TrackHit>& sortedHits,
                   const dd4hep::DDSegmentation::BitFieldCoder* decoder) {
  sortedHits.reserve(unsortedHits->size());
  for (const auto& hit : *unsortedHits) {
    sortedHits.push_back(hit);
  }
  std::sort(sortedHits.begin(), sortedHits.end(), CellIdOrder(decoder));
}

}
