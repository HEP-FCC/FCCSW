#ifndef SIMG4COMPONENTS_G4FULLSIMALG_H
#define SIMG4COMPONENTS_G4FULLSIMALG_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/IG4SaveOutputTool.h"
class IG4SimSvc;

// albers
class MCParticleCollection;

// Geant
class G4Event;

/** @class G4FullSimAlg SimG4Components/src/G4FullSimAlg.h G4FullSimAlg.h
 *
 *  Geant simulation algorithm for the full simulation.
 *  Controls the event flow: translates the EDM event to G4Event, passes it to G4SimSvc,
 *  retrieves it after the finished simulation, retrieves the hits collections and stores them.
 *  It takes MCParticleCollection (\b'genParticles') as the input.
 *  The output contains: TrackHitCollection (\b'trackClusters'),
 *  TrackClusterCollection (\b'trackHits'),
 *  TrackClusterHitsAssociationCollection (\b'trackHitsClusters'),
 *  CaloClusterCollection (\b'caloClusters'),
 *  CaloHitCollection (\b'caloHits')
 *
 *  @author Anna Zaborowska
 */

class G4FullSimAlg: public GaudiAlgorithm {
public:
  G4FullSimAlg(const std::string&, ISvcLocator*);
  virtual ~G4FullSimAlg();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Execute the simulation.
   *   Translation of MCParticleCollection to G4Event is done using EDM2G4() method.
   *   Then, G4Event is passed to G4SimSvc for the simulation and retrieved afterwards.
   *   Finally, the hits collections are retrieve to be stored and the event is terminated.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
private:
  /**  Converter from EDM to G4Event
   *   @return translated event
   */
  G4Event* EDM2G4();
  /// Handle for the EDM MC particles to be read
  DataHandle<MCParticleCollection> m_genParticles;
  /// Pointer to the interface of Geant simulation service
  SmartIF<IG4SimSvc> m_geantSvc;
  /// Handle to the tool saving the output from an event (with tracker hits)
  /// to be replaced with the ToolHandleArray<IG4SaveOutputTool> m_saveTools
  ToolHandle<IG4SaveOutputTool> m_saveTrackerTool;
  /// Handle to the tool saving the output from an event (with HCAL hits)
  /// to be replaced with the ToolHandleArray<IG4SaveOutputTool> m_saveTools
  ToolHandle<IG4SaveOutputTool> m_saveHCalTool;
};
#endif /* SIMG4COMPONENTS_G4FULLSIMALG_H */
