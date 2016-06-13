#ifndef SIMG4COMPONENTS_G4SAVETRACKERHITS_H
#define SIMG4COMPONENTS_G4SAVETRACKERHITS_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/ISimG4SaveOutputTool.h"

// datamodel
namespace fcc {
class TrackClusterCollection;
class TrackHitCollection;
class TrackClusterHitsAssociationCollection;
}

/** @class SimG4SaveTrackerHits SimG4Components/src/SimG4SaveTrackerHits.h SimG4SaveTrackerHits.h
 *
 *  Save tracker hits tool.
 *
 *  @author Anna Zaborowska
 */

class SimG4SaveTrackerHits: public GaudiTool, virtual public ISimG4SaveOutputTool {
public:
  explicit SimG4SaveTrackerHits(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~SimG4SaveTrackerHits();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save the data output.
   *   Saves the tracker hits and clusters from every hits collection associated with the event
   *   that has 'Tracker' in its name.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode saveOutput(const G4Event& aEvent) final;
private:
  /// Handle for tracker clusters
  DataHandle<fcc::TrackClusterCollection> m_trackClusters;
  /// Handle for tracker hits
  DataHandle<fcc::TrackHitCollection> m_trackHits;
  /// Handle for tracker hits-clusters associations
  DataHandle<fcc::TrackClusterHitsAssociationCollection> m_trackHitsClusters;

};

#endif /* SIMG4COMPONENTS_G4SAVETRACKERHITS_H */
