#ifndef SIMG4COMPONENTS_GEANTFULLSIMALG_H
#define SIMG4COMPONENTS_GEANTFULLSIMALG_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
namespace sim {
class IGeantSvc;
}

// albers
class MCParticleCollection;
class TrackClusterCollection;
class TrackHitCollection;
class TrackClusterHitsAssociationCollection;

// Geant
class G4Event;

namespace sim {
class GeantFullSimAlg: public GaudiAlgorithm {
public:
  GeantFullSimAlg(const std::string&, ISvcLocator*);
  virtual ~GeantFullSimAlg();
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();
    private:
  /// Converter between EDM and G4Event
  G4Event* EDM2G4();
  /// Save tracker hits.
  void SaveTrackerHits(const G4Event*);
  /// Handle for the EDM MC particles to be read
  DataHandle<MCParticleCollection> m_genParticles;
  /// Handle for tracker clusters
  DataHandle<TrackClusterCollection> m_trackClusters;
  /// Handle for tracker hits
  DataHandle<TrackHitCollection> m_trackHits;
  /// Handle for tracker hits-clusters associations
  DataHandle<TrackClusterHitsAssociationCollection> m_trackHitsClusters;
  /// Pointer to the interface of Geant Simulation service
  IGeantSvc* m_geantSvc;
};
}
#endif /* SIMG4COMPONENTS_GEANTFULLSIMALG_H */
