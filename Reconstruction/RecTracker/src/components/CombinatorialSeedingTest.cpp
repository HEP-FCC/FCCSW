
#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDRec/API/IDDecoder.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include <cmath>
#include <random>

#include "CombinatorialSeedingTest.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecTracker/TrackingUtils.h"

DECLARE_ALGORITHM_FACTORY(CombinatorialSeedingTest)

CombinatorialSeedingTest::CombinatorialSeedingTest(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc) {

  declareProperty("positionedTrackHits", m_positionedTrackHits, "Tracker hits (Input)");
  declareProperty("TrackSeedingTool", m_trackSeedingTool);
}

StatusCode CombinatorialSeedingTest::initialize() {
  info() << "initialize" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode CombinatorialSeedingTest::execute() {

  // get hits from event store
  const fcc::PositionedTrackHitCollection* hits = m_positionedTrackHits.get();
  auto seedmap = m_trackSeedingTool->findSeeds(hits);

  for (auto seedIdPair: seedmap) {
    debug() << "trackseed: " << seedIdPair.first << "\t" << seedIdPair.second << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode CombinatorialSeedingTest::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
