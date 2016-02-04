#ifndef SIMG4COMPONENTS_GEANTFULLSIMALG_H
#define SIMG4COMPONENTS_GEANTFULLSIMALG_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeantSvc;

// albers
namespace fcc {
class MCParticleCollection;
class TrackClusterCollection;
class TrackHitCollection;
class TrackClusterHitsAssociationCollection;
class CaloClusterCollection;
class CaloHitCollection;
}

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
  /// Save hadronic calorimeter deposits
  void saveHCalDeposits(const G4Event&);
  /// Save tracker hits.
  void saveTrackerHits(const G4Event&);
  /// Handle for the EDM MC particles to be read
  DataHandle<fcc::MCParticleCollection> m_genParticles;
  /// Handle for tracker clusters
  DataHandle<fcc::TrackClusterCollection> m_trackClusters;
  /// Handle for tracker hits
  DataHandle<fcc::TrackHitCollection> m_trackHits;
  /// Handle for tracker hits-clusters associations
  DataHandle<fcc::TrackClusterHitsAssociationCollection> m_trackHitsClusters;
  /// Handle for hadron calo clusters
  DataHandle<fcc::CaloClusterCollection> m_caloClusters;
  /// Handle for hadron calo hits
  DataHandle<fcc::CaloHitCollection> m_caloHits;
  /// Pointer to the interface of Geant Simulation service
  SmartIF<IGeantSvc> m_geantSvc;
};
#endif /* SIMG4COMPONENTS_GEANTFULLSIMALG_H */
