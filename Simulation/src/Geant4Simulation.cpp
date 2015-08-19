#include "Geant4Simulation.h"

// Geant4
#include "FTFP_BERT.hh"

DECLARE_COMPONENT(Geant4Simulation)

Geant4Simulation::Geant4Simulation(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc), G4RunManager()
{
   declareInput("hepmcevent", m_eventhandle);
   declareOutput("particles", m_recphandle);
}

StatusCode Geant4Simulation::initialize()
{
   if (GaudiAlgorithm::initialize().isFailure())
      return StatusCode::FAILURE;

   if (service("GeoSvc", m_geoSvc, true).isFailure())
   {
      error() << "Couldn't GeoSvc" << endmsg;
      return StatusCode::FAILURE;
   }

   // load standard physics list (full G4 sim)
   // deleted in ~G4RunManager()
   G4RunManager::SetUserInitialization(new FTFP_BERT);

   // take geometry (DD4Hep)
   // deleted in ~G4RunManager()
   G4RunManager::SetUserInitialization(m_geoSvc->getGeant4Geo());

   G4RunManager::Initialize();
   // as in G4RunManager::BeamOn()
   if(G4RunManager::ConfirmBeamOnCondition())
   {
      G4RunManager::numberOfEventToBeProcessed = 1;
      G4RunManager::ConstructScoringWorlds();
      G4RunManager::RunInitialization();
      return StatusCode::SUCCESS;
   }
   else
   {
      error() << "Geant hasn't been initialised correctly." << endmsg;
      return StatusCode::FAILURE;
   }
}

StatusCode Geant4Simulation::execute() {
   //read event
   auto hepmc_event = m_eventhandle.get();

   G4RunManager::currentEvent = HepMC2G4(hepmc_event);
   G4RunManager::eventManager->ProcessOneEvent(G4RunManager::currentEvent);
   G4RunManager::AnalyzeEvent(G4RunManager::currentEvent);
   G4RunManager::UpdateScoring();
   G4RunManager::TerminateOneEvent();

   // ParticleCollection* particles = new ParticleCollection();
   // m_recphandle.put(particles);

   return StatusCode::SUCCESS;
}

StatusCode Geant4Simulation::finalize() {
   G4RunManager::RunTermination();
   return GaudiAlgorithm::finalize();
}

G4Event* Geant4Simulation::HepMC2G4(const HepMC::GenEvent* aHepMCEvent)
{
   G4Event* aG4Event = new G4Event;
   for(auto vertex_i = aHepMCEvent->vertices_begin();
       vertex_i != aHepMCEvent->vertices_end(); ++vertex_i )
   {
      // check if the vertex is valid
      bool true_vertex=false;
      for (auto particle_i= (*vertex_i)->particles_begin(HepMC::children);
           particle_i != (*vertex_i)->particles_end(HepMC::children); ++particle_i)
      {
         if (!(*particle_i)->end_vertex() && (*particle_i)->status()==1)
         {
            true_vertex=true;
            break;
         }
      }
      if (!true_vertex) continue;

      HepMC::FourVector tmp= (*vertex_i)->position();
      G4LorentzVector vertex_pos(tmp.x(), tmp.y(), tmp.z(), tmp.t());

      // create G4PrimaryVertex and associated G4PrimaryParticles
      G4PrimaryVertex* g4_vertex= new G4PrimaryVertex(vertex_pos.x()*cm, vertex_pos.y()*cm, vertex_pos.z()*cm, vertex_pos.t()*cm/c_light);

      for (auto particle_i= (*vertex_i)->particles_begin(HepMC::children);
           particle_i != (*vertex_i)->particles_end(HepMC::children); ++particle_i) {
         if( (*particle_i)->status() != 1 ) continue;

         int pdgcode= (*particle_i)-> pdg_id();
         tmp= (*particle_i)-> momentum();
         G4LorentzVector mom(tmp.px(), tmp.py(), tmp.pz(), tmp.e());
         G4PrimaryParticle* g4_particle=
            new G4PrimaryParticle(pdgcode, mom.x()*GeV, mom.y()*GeV, mom.z()*GeV);
         g4_vertex->SetPrimary(g4_particle);
      }
      aG4Event->AddPrimaryVertex(g4_vertex);
   }
   return aG4Event;
}

