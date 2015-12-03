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
GaudiAlgorithm(name, svcLoc) {
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
   m_runManager.SetUserInitialization(m_geantConfigTool->getPhysicsList());

   // Take geometry (from DD4Hep), deleted in ~G4RunManager()
   m_runManager.SetUserInitialization(m_geoSvc->getGeant4Geo());

   // Attach user actions
    m_runManager.SetUserInitialization(m_geantConfigTool->getActionInitialization());
    m_runManager.Initialize();

   m_geantConfigTool->getOtherSettings();

   if( !m_runManager.start())
   {
      error() << "Unable to initialize GEANT correctly." << endmsg;
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}

StatusCode Geant4Simulation::execute() {
   // first translate event
  G4Event* event = EDM2G4();
   if ( !event ) {
      error() << "Unable to translate EDM MC data to G4Event" << endmsg;
      return StatusCode::FAILURE;
   }
   bool status = m_runManager.processEvent( event );
   if ( !status ) {
      error() << "Unable to process event in Geant" << endmsg;
      return StatusCode::FAILURE;
   }
   const G4Event* constevent;
   status = m_runManager.retrieveEvent(constevent);
   m_runManager.terminateEvent();
   return StatusCode::SUCCESS;
}

StatusCode Geant4Simulation::finalize() {
   m_runManager.finalize();
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

