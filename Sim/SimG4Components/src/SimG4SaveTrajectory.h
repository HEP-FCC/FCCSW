#ifndef SIMG4COMPONENTS_G4SAVETRAJECTORY
#define SIMG4COMPONENTS_G4SAVETRAJECTORY

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/ISimG4SaveOutputTool.h"
class IGeoSvc;

// datamodel
namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}

/** @class SimG4SaveTrajectory SimG4Components/src/SimG4SaveTrajectory.h SimG4SaveTrajectory.h
 *
 *
 *  @author vavolkl
 */

class SimG4SaveTrajectory : public GaudiTool, virtual public ISimG4SaveOutputTool {
public:
  explicit SimG4SaveTrajectory(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimG4SaveTrajectory();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save the data output.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode saveOutput(const G4Event& aEvent) final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Handle for tracker hits including position information
  DataHandle<fcc::TrackHitCollection> m_trackHits{"hits/trajectory",
                                                                      Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::PositionedTrackHitCollection> m_positionedTrackHits{"hits/trajectoryPoints",
                                                                      Gaudi::DataHandle::Writer, this};
};

#endif /* SIMG4COMPONENTS_G4SAVETRAJECTORY */
