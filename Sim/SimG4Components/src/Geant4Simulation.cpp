#include "Geant4Simulation.h"

// FCCSW
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Common/Units.h"
#include "DetDesInterfaces/IGeoSvc.h"
#include "SimG4Interface/IGeantConfigTool.h"

// albers
#include "datamodel/ParticleCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/ParticleMCAssociationCollection.h"

// Geant
#include "G4VModularPhysicsList.hh"

DECLARE_COMPONENT(Geant4Simulation)

Geant4Simulation::Geant4Simulation(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc), G4RunManager() {
   declareInput("genparticles", m_genphandle);
   declareOutput("particles", m_recphandle);
   declareOutput("particleassociation", m_partassociationhandle);
   declareProperty ("config", m_geantConfigName = "" ) ;
}

StatusCode Geant4Simulation::initialize() {
   // Initialization - Gaudi part
   if (GaudiAlgorithm::initialize().isFailure())
      return StatusCode::FAILURE;
   if (service("GeoSvc", m_geoSvc, true).isFailure()) {
      error() << "Unable to locate Geometry Service" << endmsg;
      return StatusCode::FAILURE;
   }
   m_geantConfigTool = tool<IGeantConfigTool>(m_geantConfigName);   // Initialization - Geant part
   // Load physics list, deleted in ~G4RunManager()
   G4VModularPhysicsList* physics_list = m_geantConfigTool->getPhysicsList();
   // Deleted in ~G4VModularPhysicsList()
   G4RunManager::SetUserInitialization(physics_list);

   // Take geometry (from DD4Hep), deleted in ~G4RunManager()
   G4RunManager::SetUserInitialization(m_geoSvc->getGeant4Geo());

   // Attach user actions
   G4RunManager::SetUserInitialization(m_geantConfigTool->getActionInitialization());
   G4RunManager::Initialize();

   m_geantConfigTool->getOtherSettings();

   // as in G4RunManager::BeamOn()
   if(G4RunManager::ConfirmBeamOnCondition()) {
      G4RunManager::ConstructScoringWorlds();
      G4RunManager::RunInitialization();
      return StatusCode::SUCCESS;
   }
   else {
      error() << "Unable to initialize GEANT correctly." << endmsg;
      return StatusCode::FAILURE;
   }
}

StatusCode Geant4Simulation::execute() {
   //read event
   G4RunManager::currentEvent = EDM2G4();
   if ( !G4RunManager::currentEvent ) {
      error() << "Unable to translate EDM MC data to G4Event" << endmsg;
      return StatusCode::FAILURE;
   }
   G4RunManager::eventManager->ProcessOneEvent(G4RunManager::currentEvent);
   G4RunManager::AnalyzeEvent(G4RunManager::currentEvent);
   G4RunManager::UpdateScoring();
   G4RunManager::TerminateOneEvent();

   return StatusCode::SUCCESS;
}

StatusCode Geant4Simulation::finalize() {
   G4RunManager::RunTermination();
   return GaudiAlgorithm::finalize();
}

G4Event* Geant4Simulation::EDM2G4() {
   // Event will be passed to G4RunManager and be deleted in G4RunManager::RunTermination()
   G4Event* g4_event = new G4Event();
   // Creating EDM collections
   const MCParticleCollection* mcparticles = m_genphandle.get();
   ParticleCollection* particles = new ParticleCollection();
   ParticleMCAssociationCollection* associations = new ParticleMCAssociationCollection();
   // Adding one particle per one vertex => vertices repeated
   for(const auto& mcparticle : *mcparticles) {
      const GenVertex& v = mcparticle.read().StartVertex.read();
      G4PrimaryVertex* g4_vertex = new G4PrimaryVertex
         (v.Position.X*edm2g4::length, v.Position.Y*edm2g4::length, v.Position.Z*edm2g4::length, v.Ctau*edm2g4::length);
      const BareParticle& mccore = mcparticle.read().Core;
      G4PrimaryParticle* g4_particle = new G4PrimaryParticle
         (mccore.Type, mccore.P4.Px*edm2g4::energy, mccore.P4.Py*edm2g4::energy, mccore.P4.Pz*edm2g4::energy);
      ParticleHandle particle = particles->create();
      g4_particle->SetUserInformation(new ParticleInformation(mcparticle, particle));
      ParticleMCAssociationHandle association = associations->create();
      association.mod().Rec = particle;
      association.mod().Sim = mcparticle;
      g4_vertex->SetPrimary(g4_particle);
      g4_event->AddPrimaryVertex(g4_vertex);
   }
   m_recphandle.put(particles);
   m_partassociationhandle.put(associations);
   return g4_event;
}

