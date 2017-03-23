#include "TruthSeedingTool.h"

DECLARE_TOOL_FACTORY(TruthSeedingTool)

TruthSeedingTool::TruthSeedingTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ITrackSeedingTool>(this);
}

StatusCode TruthSeedingTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  return sc;
}




std::multimap<unsigned int, unsigned int> TruthSeedingTool::findSeeds( const fcc::PositionedTrackHitCollection* theHits) {
  std::multimap<unsigned int, unsigned int> theSeeds;
  int hitCounter = 0;
  // map trackIDs to indices in hit collection
  for (auto hit: *theHits) {
    theSeeds.insert( std::pair<unsigned int, int>(hit.core().bits, hitCounter));
    ++hitCounter;
  }
  return theSeeds;
}

StatusCode TruthSeedingTool::finalize() { return GaudiTool::finalize(); }
