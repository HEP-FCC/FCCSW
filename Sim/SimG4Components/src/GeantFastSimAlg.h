#ifndef SIMG4COMPONENTS_GEANTFASTSIMALG_H
#define SIMG4COMPONENTS_GEANTFASTSIMALG_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeantSvc;

// albers
class MCParticleCollection;
class ParticleCollection;
class ParticleMCAssociationCollection;

// Geant
class G4Event;

class GeantFastSimAlg: public GaudiAlgorithm {
public:
  GeantFastSimAlg(const std::string&, ISvcLocator*);
  virtual ~GeantFastSimAlg();
  /// Initialize.
  virtual StatusCode initialize() final;
  /// Execute.
  virtual StatusCode execute() final;
  /// Finalize.
  virtual StatusCode finalize() final;
  private:
  /// Converter between EDM and G4Event
  G4Event* EDM2G4();
  // saving methods......
  /// Handle for the EDM MC particles to be read
  DataHandle<MCParticleCollection> m_genParticles;
  /// Handle for the particles to be written
  DataHandle<ParticleCollection> m_recphandle;
  /// Handle for the associations between particles and MC particles to be written
  DataHandle<ParticleMCAssociationCollection> m_partassociationhandle;
  /// Pointer to the interface of Geant Simulation service
  SmartIF<IGeantSvc> m_geantSvc;
};
#endif /* SIMG4COMPONENTS_GEANTFASTSIMALG_H */
