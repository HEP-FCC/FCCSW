#ifndef _GEANT4SIMULATION_H_
#define _GEANT4SIMULATION_H_

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Common/RunManager.h"
class IGeoSvc;
class IGeantConfigTool;
class IG4IOTool;

// albers
class MCParticleCollection;
class ParticleCollection;
class ParticleMCAssociationCollection;
class TrackClusterCollection;

class Geant4Simulation: public GaudiAlgorithm {
   friend class AlgFactory<Geant4Simulation> ;
public:
   /// Constructor.
   Geant4Simulation(const std::string& name, ISvcLocator* svcLoc);
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
   DataHandle<MCParticleCollection> m_genphandle;
   /// Handle for the particles to be written
   DataHandle<ParticleCollection> m_recphandle;
   /// Handle for the associations between particles and MC particles to be written
   DataHandle<ParticleMCAssociationCollection> m_partassociationhandle;
   /// Handle for tracker clusters
   DataHandle<TrackClusterCollection> m_trackClusters;
   /// Geant Run Manager
   sim::RunManager m_runManager;
   /// Pointer to the interface of geometry service
   IGeoSvc* m_geoSvc;
   /// Pointer to the Geant configuration tool
   IGeantConfigTool* m_geantConfigTool;
   /// Name of the ISmearingTool (set by options)
   std::string m_geantConfigName;
   /// Pointer to the Geant IO tool
   IG4IOTool* m_geantIOTool;
   /// Name of the IG4IOTool (set by options)
   std::string m_geantIOToolName;
};

#endif
