#ifndef SIMG4COMPONENTS_G4FASTSIMALG_H
#define SIMG4COMPONENTS_G4FASTSIMALG_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IG4SimSvc;

// albers
class MCParticleCollection;
class ParticleCollection;
class ParticleMCAssociationCollection;

// Geant
class G4Event;

/** @class G4FastSimAlg SimG4Components/src/G4FastSimAlg.h G4FastSimAlg.h
 *
 *  Geant simulation algorithm for the fast simulation.
 *  Controls the event flow: translates the EDM event to G4Event, passes it to G4SimSvc.
 *  The output, 'reconstructed' particles are stored from the user tracking action.
 *  It takes MCParticleCollection (\b'genParticles') as the input.
 *  The output contains: ParticleCollection (\b'particles'),
 *  ParticleMCAssociationCollection (\b'particleassociation').
 *
 *  @author Anna Zaborowska
 */

class G4FastSimAlg: public GaudiAlgorithm {
public:
  G4FastSimAlg(const std::string&, ISvcLocator*);
  virtual ~G4FastSimAlg();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Execute the simulation.
   *   Translation of MCParticleCollection to G4Event is done using EDM2G4() method.
   *   Then, G4Event is passed to G4SimSvc for the simulation.
   *   Finally, the event is terminated.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  private:
  /**  Converter from EDM to G4Event
   *   The output is created and associated with the Monte Carlo particles.
   *   Handles to MCParticle and Particle are stored inside G4Event (using G4VUserPrimaryParticleInformation).
   *   @return translated event
   */
  G4Event* EDM2G4();
  /// Handle for the EDM MC particles to be read
  DataHandle<MCParticleCollection> m_genParticles;
  /// Handle for the particles to be written
  DataHandle<ParticleCollection> m_recphandle;
  /// Handle for the associations between particles and MC particles to be written
  DataHandle<ParticleMCAssociationCollection> m_partassociationhandle;
  /// Pointer to the interface of Geant simulation service
  SmartIF<IG4SimSvc> m_geantSvc;
};
#endif /* SIMG4COMPONENTS_G4FASTSIMALG_H */
