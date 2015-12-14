#ifndef SIMG4COMPONENTS_GEANTFULLSIMALG_H
#define SIMG4COMPONENTS_GEANTFULLSIMALG_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeantSvc;

// albers
class MCParticleCollection;
class TrackClusterCollection;
class TrackHitCollection;
class TrackClusterHitsAssociationCollection;

// Geant
class G4Event;

class GeantFullSimAlg: public GaudiAlgorithm {
public:
  GeantFullSimAlg(const std::string&, ISvcLocator*);
  virtual ~GeantFullSimAlg();
  /// Initialize.
  virtual StatusCode initialize() final;
  /// Execute.
  virtual StatusCode execute() final;
  /// Finalize.
  virtual StatusCode finalize() final;
private:
  /// Converter between EDM and G4Event
  G4Event* EDM2G4();
  /// Save tracker hits.
  void saveTrackerHits(const G4Event*);
  /// Handle for the EDM MC particles to be read
  DataHandle<MCParticleCollection> m_genParticles;
  /// Handle for tracker clusters
  DataHandle<TrackClusterCollection> m_trackClusters;
  /// Handle for tracker hits
  DataHandle<TrackHitCollection> m_trackHits;
  /// Handle for tracker hits-clusters associations
  DataHandle<TrackClusterHitsAssociationCollection> m_trackHitsClusters;
  /// Pointer to the interface of Geant Simulation service
  SmartIF<IGeantSvc> m_geantSvc;
};
#endif /* SIMG4COMPONENTS_GEANTFULLSIMALG_H */
