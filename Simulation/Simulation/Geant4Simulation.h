#ifndef _GEANT4SIMULATION_H_
#define _GEANT4SIMULATION_H_

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;
class ISmearingTool;

// albers
class ParticleCollection;
class MCParticleCollection;
class ParticleMCAssociationCollection;

// Geant4
#include "G4RunManager.hh"
class G4VFastSimulationModel;

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
   /// Pointer to the smearing tool
   ISmearingTool* m_smearTool;
   /// Name of the ISmearingTool (set by options)
   std::string m_smearToolName ;
   /// Switch full/fast sim (set by options)
   std::string m_simtype;
   /// Switch full/fast sim (used in the code)
   SimType m_type;
   /// Envelopes that are used in a parametric simulation
   /// deleted by the G4RegionStore
   std::vector<G4Region*> m_g4regions;
   /// Fast Simulation Models
   std::vector<std::unique_ptr<G4VFastSimulationModel>> m_models;
};

#endif
