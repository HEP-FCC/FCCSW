#ifndef RECTRACKER_COMBINATORIALSEEDINGTEST_H
#define RECTRACKER_COMBINATORIALSEEDINGTEST_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"

class IGeoSvc;
class ITrackSeedingTool;

namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}


/*** @class CombinatorialSeedingTest 
 *
 *  Algorithm that only calls the Combinatorial Seeding tool
 *  for testing purposes and pints a debug statement.
 */
class CombinatorialSeedingTest : public GaudiAlgorithm {
public:
  CombinatorialSeedingTest(const std::string& name, ISvcLocator* svcLoc);

  ~CombinatorialSeedingTest() = default;

  StatusCode initialize() override final;

  StatusCode execute() override final;

  StatusCode finalize() override final;

private:
  /// TrackHits as Input to the track seeding
  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedTrackHits", Gaudi::DataHandle::Reader,
                                                                      this};
  /// Handle to Track Seeding Tool that does the work
  ToolHandle<ITrackSeedingTool> m_trackSeedingTool{"CombinatorialSeedingTool/CombinatorialSeedingTool", this};
};

#endif /* RECTRACKER_COMBINATORIALSEEDINGTEST_H */
