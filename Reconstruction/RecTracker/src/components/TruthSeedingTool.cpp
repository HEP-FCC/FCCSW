

#include "TruthSeedingTool.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"


DECLARE_TOOL_FACTORY(TruthSeedingTool)

TruthSeedingTool::TruthSeedingTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent)
       {
  declareInterface<ITrackSeedingTool>(this);
}

StatusCode TruthSeedingTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  return sc;
}


std::multimap<unsigned int, unsigned int>
TruthSeedingTool::findSeeds(const fcc::PositionedTrackHitCollection* theHits) {

  // map trackId -> hitIndex
  std::multimap<unsigned int, unsigned int> theSeeds;

  unsigned int hitCounter = 0; // serves as index of hits in collection
  for (auto hit : *theHits) {
      // use mc-truth information (track-id saved in "bits" member) to label tracks
      // unfortunately not available for real data ...
      theSeeds.insert(std::pair<unsigned int, unsigned int>(hit.bits(), hitCounter));
    ++hitCounter;
    }

  return theSeeds;
}

StatusCode TruthSeedingTool::finalize() { return GaudiTool::finalize(); }
