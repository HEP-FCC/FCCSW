#include "Geant4Simulation.h"

// Geant4
#include "FTFP_BERT.hh"
#include "G4SystemOfUnits.hh"
#include "G4RegionStore.hh"
#include "G4VModularPhysicsList.hh"

#include "GeantFast/FastSimPhysics.h"
#include "GeantFast/TrackingAction.h"

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
      error() << "Couldn't get GeoSvc" << endmsg;
      return StatusCode::FAILURE;
   }
   // load standard physics list (full G4 sim)
   // deleted in ~G4RunManager()
   G4VModularPhysicsList* physics_list = new FTFP_BERT;
   // attach Fast Simulation Process (will take over normal transportation if FastSimModel triggered
   // deleted in ~G4VModularPhysicsList()
   physics_list->RegisterPhysics(new FastSimPhysics);
   G4RunManager::SetUserInitialization(physics_list);
   // G4RunManager::SetUserAction(new TrackingAction);

   // take geometry (DD4Hep)
   // deleted in ~G4RunManager()
   G4RunManager::SetUserInitialization(m_geoSvc->getGeant4Geo());
   G4RunManager::Initialize();

   // create envelopes for the geometry
   for(int iter_region = 0; iter_region<(*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume()->GetNoDaughters(); ++iter_region)
   {
      m_g4regions.emplace_back(new G4Region((*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume()->GetDaughter(iter_region)->GetName()+"_fastsim"));
      m_g4regions[iter_region]->AddRootLogicalVolume((*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume()->GetDaughter(iter_region)->GetLogicalVolume());
   }
   G4RegionStore* region_store = G4RegionStore::GetInstance();
   if (region_store)
      for(auto& i:*region_store)
      {
         if(i->GetName().find("fastsim") != std::string::npos)
         {
            m_models.emplace_back(new FastSimModelTest(i->GetName(),i));
            info()<<"Attaching a Fast Simulation Model to the region "<<i->GetName()<<endmsg;
         }
      }
   // as in G4RunManager::BeamOn()
   if(G4RunManager::ConfirmBeamOnCondition())
   {
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
   if ( !hepmc_event->is_valid() )
   {
      error() << "Couldn't get event" << endmsg;
      return StatusCode::FAILURE;
   }
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

G4Event* Geant4Simulation::HepMC2G4(const HepMC::GenEvent* aHepMC_event)
{
   G4Event* g4_event = new G4Event();
   double length_unit = HepMC::Units::conversion_factor(aHepMC_event->length_unit(), HepMC::Units::MM)*mm;
   double mom_unit = HepMC::Units::conversion_factor(aHepMC_event->momentum_unit(),HepMC::Units::GEV)*GeV;

   for(auto vertex_i = aHepMC_event->vertices_begin();
       vertex_i != aHepMC_event->vertices_end(); ++vertex_i )
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
      G4PrimaryVertex* g4_vertex= new G4PrimaryVertex(vertex_pos.x()*length_unit, vertex_pos.y()*length_unit,
                                                      vertex_pos.z()*length_unit, vertex_pos.t()*length_unit/c_light);

      for (auto particle_i= (*vertex_i)->particles_begin(HepMC::children);
           particle_i != (*vertex_i)->particles_end(HepMC::children); ++particle_i) {
         if( (*particle_i)->status() != 1 ) continue;

         int pdgcode= (*particle_i)-> pdg_id();
         tmp= (*particle_i)-> momentum();
         G4LorentzVector mom(tmp.px(), tmp.py(), tmp.pz(), tmp.e());
         G4PrimaryParticle* g4_particle=
            new G4PrimaryParticle(pdgcode, mom.x()*mom_unit, mom.y()*mom_unit, mom.z()*mom_unit);
         g4_vertex->SetPrimary(g4_particle);
      }
      g4_event->AddPrimaryVertex(g4_vertex);
   }
   return g4_event;
}

