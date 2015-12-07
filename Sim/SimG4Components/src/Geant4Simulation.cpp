#include "Geant4Simulation.h"
// tmp
#include <fstream>

// FCCSW
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Common/Units.h"
#include "DetDesInterfaces/IGeoSvc.h"
#include "SimG4Interface/IGeantConfigTool.h"
#include "SimG4Interface/IG4IOTool.h"
#include "SimG4Data/GeantTrackerHit.h"

// albers
#include "datamodel/MCParticleCollection.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCAssociationCollection.h"
#include "datamodel/TrackClusterCollection.h"

// Geant
#include "G4VModularPhysicsList.hh"
#include "G4HCofThisEvent.hh"

DECLARE_COMPONENT(Geant4Simulation)

Geant4Simulation::Geant4Simulation(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) {
   declareInput("genparticles", m_genphandle);
   declareOutput("particles", m_recphandle);
   declareOutput("particleassociation", m_partassociationhandle);
   declareOutput("trackclusters", m_trackClusters);
   declareProperty ("config", m_geantConfigName = "" ) ;
   declareProperty ("io", m_geantIOToolName = "" ) ;
}

StatusCode Geant4Simulation::initialize() {
   // Initialization - Gaudi part
   if (GaudiAlgorithm::initialize().isFailure())
      return StatusCode::FAILURE;
   if (service("GeoSvc", m_geoSvc, true).isFailure()) {
      error() << "Unable to locate Geometry Service" << endmsg;
      return StatusCode::FAILURE;
   }
   m_geantConfigTool = tool<IGeantConfigTool>(m_geantConfigName);
   if(!m_geantIOToolName.empty()) {
     m_geantIOTool = tool<IG4IOTool>(m_geantIOToolName);
   }
   // Initialization - Geant part
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
   SaveTrackerHits(constevent);
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

void Geant4Simulation::SaveTrackerHits(const G4Event* aEvent) {
  G4HCofThisEvent* collections = aEvent->GetHCofThisEvent();
  GeantTrackerHitsCollection* track_collection;
  if(collections) {
    track_collection = (GeantTrackerHitsCollection*)(collections->GetHC(0));
    if(track_collection) {
      // HERE: Save hits to the EDM?
      int n_hit = track_collection->entries();
      debug() << "     " << n_hit
             << " hits are stored in SaveHitsTrackerHitsCollection" << endmsg;
      // tmp solution
      std::ofstream file;
      CLHEP::Hep3Vector pos;
      file.open("tracker_hits.txt");
      for(auto iter_hit=0; iter_hit<n_hit; iter_hit++ ) {
        pos = dynamic_cast<GeantTrackerHit*>(track_collection->GetHit(iter_hit))->GetPosition();
        file<<pos.x()<<" "<<pos.y()<<" "<<pos.z()<<std::endl;
      }
      file.close();
    }
  }
}
