#ifndef _GEANT4SIMULATION_H_
#define _GEANT4SIMULATION_H_

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "DetDesInterfaces/IGeoSvc.h"
#include "FWCore/DataHandle.h"
#include "GeantFast/FastSimModelTest.h"
#include "GeantComponents/ISmearingTool.h"

// albers
#include "datamodel/ParticleCollection.h"
#include "datamodel/Particle.h"

// Geant4
#include "G4RunManager.hh"
#include "G4Event.hh"

// HepMC
#include "HepMC/GenEvent.h"

class Geant4Simulation: public GaudiAlgorithm , public G4RunManager{
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
   /// Converter between HepMC::GenEvent and G4Event
   G4Event* HepMC2G4(const HepMC::GenEvent* aHepMCEvent) const;
   /// Handle for the HepMC event to be read
   DataHandle<HepMC::GenEvent> m_eventhandle;
   /// Handle for the particles to be written
   DataHandle<ParticleCollection> m_recphandle;
   /// Pointer to the interface of geometry service
   IGeoSvc* m_geoSvc;
   /// Name of the ISmearingTool (set by options)
   std::string m_smearToolName ;
   /// Pointer to the smearing tool
   ISmearingTool* m_smearTool;
   /// Switch full/fast sim (set by options)
   std::string m_simtype;
   /// Switch full/fast sim (used further)
   SimType m_type;
   /// Envelopes that are used in a parametric simulation
   /// deleted by the G4RegionStore
   std::vector<G4Region*> m_g4regions;
   /// Fast Simulation Models
   std::vector<std::unique_ptr<G4VFastSimulationModel>> m_models;
};

#endif
