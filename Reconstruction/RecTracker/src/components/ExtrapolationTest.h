#ifndef RECTRACKER_EXTRAPOLATIONTEST_H
#define RECTRACKER_EXTRAPOLATIONTEST_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/IExtrapolationTool.h"

namespace fcc {
class PositionedTrackHitCollection;
class MCParticleCollection;
}

/** @class ExtrapolationTest
 *
 *  @brief Extrapolates particles thorugh the tracker
 *
 * The Extrapolationtest extrapolates given particles through the tracking detector and writes out positionedTrackHits
 * using the ExtrapolationTool.
 *
 * @author Valentin Volkl, Julia Hrdinka
 *
 */

class ExtrapolationTest : public GaudiAlgorithm {
public:
  // Constructor
  ExtrapolationTest(const std::string& name, ISvcLocator* svcLoc);
  // Destructor
  ~ExtrapolationTest() = default;
  /// Gaudi interface initialize method
  StatusCode initialize() override final;
  /// Gaudi interface execute method
  StatusCode execute() override final;
  /// Gaudi interface finalize method
  StatusCode finalize() override final;

private:
  /// extrapolation tool
  ToolHandle<IExtrapolationTool> m_extrapolationTool{"ExtrapolationTool", this};
  /// Handle for the EDM MC particles to be read in
  DataHandle<fcc::MCParticleCollection> m_genParticles{"allGenParticles", Gaudi::DataHandle::Reader, this};
  /// output of points along track
  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"positionedHits", Gaudi::DataHandle::Writer,
                                                                      this};
};

#endif /* RECTRACKER_EXTRAPOLATIONTEST_H */
