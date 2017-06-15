
#include "RecTracker/TrackingUtils.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDRec/API/IDDecoder.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"


namespace rec {

// For use with standard sorting algorithms
// compares the system fields of the hits first, then the layer field
struct CellIdOrder {
  CellIdOrder(DD4hep::DDSegmentation::BitField64* dec) : m_decoder(dec) {}

  bool operator()(const fcc::TrackHit& h1, const fcc::TrackHit& h2) {
    m_decoder->setValue(h1.cellId());
    int system1 = (*m_decoder)["system"];
    int layer1 = (*m_decoder)["layer"];
    m_decoder->setValue(h2.cellId());
    int system2 = (*m_decoder)["system"];
    int layer2 = (*m_decoder)["layer"];
    if (system1 < system2) return true;
    if (system2 < system1) return false;
    return (layer1 < layer2);
  }

private:
  DD4hep::DDSegmentation::BitField64* m_decoder;
};

/// fill vector with hits ordered according to the CellIdOrder struct
void sortTrackHits(const fcc::TrackHitCollection* unsortedHits, std::vector<fcc::TrackHit>& sortedHits,
                   DD4hep::DDSegmentation::BitField64* decoder) {
  sortedHits.reserve(unsortedHits->size());
  for (const auto& hit : *unsortedHits) {
    sortedHits.push_back(hit);
  }
  std::sort(sortedHits.begin(), sortedHits.end(), CellIdOrder(decoder));
}

}
