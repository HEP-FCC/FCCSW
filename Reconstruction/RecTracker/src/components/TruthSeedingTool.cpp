

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

  std::multimap<unsigned int, unsigned int> theSeeds;

  unsigned int hitCounter = 0;
  for (auto hit : *theHits) {
      theSeeds.insert(std::pair<unsigned int, unsigned int>(hit.bits(), hitCounter));
    ++hitCounter;
    }

  return theSeeds;
}

StatusCode TruthSeedingTool::finalize() { return GaudiTool::finalize(); }
