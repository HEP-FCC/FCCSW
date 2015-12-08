#ifndef _GEANT4SIMULATION_H_
#define _GEANT4SIMULATION_H_

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;
class IGeantConfigTool;

// albers
class ParticleCollection;
class MCParticleCollection;
class ParticleMCAssociationCollection;

// Geant4
#include "G4RunManager.hh"

class Geant4Simulation: public GaudiAlgorithm , public G4RunManager {
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
   enum SimType { FULL, FAST };
private:
   /// Converter between EDM and G4Event
   G4Event* EDM2G4();
   /// Handle for the EDM MC particles to be read
   DataHandle<MCParticleCollection> m_genphandle;
   /// Handle for the particles to be written
   DataHandle<ParticleCollection> m_recphandle;
   /// Handle for the associations between particles and MC particles to be written
   DataHandle<ParticleMCAssociationCollection> m_partassociationhandle;
   /// Pointer to the interface of geometry service
   IGeoSvc* m_geoSvc;
   /// Pointer to the Geant configuration tool
   IGeantConfigTool* m_geantConfigTool;
   /// Name of the ISmearingTool (set by options)
   std::string m_geantConfigName;
};

#endif
