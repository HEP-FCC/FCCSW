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

class CombinatorialSeedingTest : public GaudiAlgorithm {
public:
  CombinatorialSeedingTest(const std::string& name, ISvcLocator* svcLoc);

  ~CombinatorialSeedingTest();

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Pointer to the geometry service

  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedTrackHits", Gaudi::DataHandle::Reader,
                                                                      this};
  ToolHandle<ITrackSeedingTool> m_trackSeedingTool{"CombinatorialSeedingTool/CombinatorialSeedingTool", this};
};

#endif /* RECTRACKER_COMBINATORIALSEEDINGTEST_H */
