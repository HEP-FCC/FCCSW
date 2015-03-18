#include "Geant4Simulation.h"

#include "B1DetectorConstruction.h"
#include "FTFP_BERT.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"

DECLARE_COMPONENT(Geant4Simulation)

Geant4Simulation::Geant4Simulation(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc)
{
   declareInput("g4event", m_eventhandle);
   // declareInput("g4detector", m_g4detector);
   // declareOutput("particles", m_recphandle);
}

StatusCode Geant4Simulation::initialize() {
   GaudiAlgorithm::initialize();
   m_runManager = new G4RunManager;
   // take geometry
   ///.... from Service - check with Julia code, currently...
   m_runManager->SetUserInitialization(new B1DetectorConstruction);
   // load physics list
   m_runManager->SetUserInitialization(new FTFP_BERT);
   // initialization
   m_runManager->Initialize();
   m_runManager->RunInitialization();
	return StatusCode::SUCCESS;
}

StatusCode Geant4Simulation::execute() {
   //read event
   G4Event* event = m_eventhandle.get()->getEvent();

   // run geant
   G4EventManager* eventManager = G4EventManager::GetEventManager();
   eventManager->ProcessOneEvent(event);

   // ParticleCollection* particles = new ParticleCollection();
   // m_recphandle.put(particles);
   return StatusCode::SUCCESS;
}

StatusCode Geant4Simulation::finalize() {
   m_runManager->RunTermination();
   delete  m_runManager;
   return GaudiAlgorithm::finalize();
}
