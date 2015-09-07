#include "Geant4Simulation.h"

// Geant4
#include "FTFP_BERT.hh"
#include "G4SystemOfUnits.hh"
#include "G4RegionStore.hh"
#include "G4VModularPhysicsList.hh"

#include "GeantFast/FastSimPhysics.h"
// #include "GeantFast/FastSimPhysicsList.h"
#include "GeantFast/TrackingAction.h"

DECLARE_COMPONENT(Geant4Simulation)

Geant4Simulation::Geant4Simulation(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc), G4RunManager(), m_type(SimType::FULL)
{
   declareInput("hepmcevent", m_eventhandle);
   declareOutput("particles", m_recphandle);
   declareProperty("simtype", m_simtype = "full");
  declareProperty ("smearingtoolname", m_smearToolName = "" ) ;
}

StatusCode Geant4Simulation::initialize()
{
   // Initialization - Gaudi part
   if (GaudiAlgorithm::initialize().isFailure())
      return StatusCode::FAILURE;
   if (service("GeoSvc", m_geoSvc, true).isFailure())
   {
      error() << "Unable to locate Geometry Service" << endmsg;
      return StatusCode::FAILURE;
   }
   if( !m_simtype.compare("fast"))
      m_type = SimType::FAST;
   else if( !m_simtype.compare("full"))
      m_type = SimType::FULL;
   else
   {
      error() << "Simulation type set to invalid value <<"<<m_simtype<<">>. Possible values: <<fast>> or <<full>>."<< endmsg;
      return StatusCode::FAILURE;
   }
   info()<<"Simulation type set to (0-full, 1-fast): "<<m_type<<endmsg;

   if ( m_smearToolName.compare("") )
      if(m_type == SimType::FAST)
        m_smearTool = tool<ISmearingTool>(m_smearToolName, this);
      else
         info()<<"No smearing tool can be used in Geant FULL sim."<<endmsg;
   else
      if(m_type == SimType::FAST)
         info()<<"No smearing tool will be used."<<endmsg;

   // Initialization - Geant part
   // Load physics list, deleted in ~G4RunManager()
   G4VModularPhysicsList* physics_list = new FTFP_BERT;
   if(m_type == SimType::FAST)
   {
      // Coupled transportation enables calculation of particle trajectory in envelopes with fast sim models attached
      G4PhysicsListHelper::GetPhysicsListHelper()->UseCoupledTransportation();
      // Attach Fast Simulation Process (will take over normal transportation if FastSimModel triggered
      physics_list->RegisterPhysics(new FastSimPhysics);
   }
   // Deleted in ~G4VModularPhysicsList()
   G4RunManager::SetUserInitialization(physics_list);

   // Take geometry (from DD4Hep), deleted in ~G4RunManager()
   G4RunManager::SetUserInitialization(m_geoSvc->getGeant4Geo());
   G4RunManager::Initialize();

   if(m_type == SimType::FAST)
   {
      // Create the envelopes for the geometry, attach fast sim models for Tracker, EMCal, HCal
      for(int iter_region = 0; iter_region<(*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume()->GetNoDaughters(); ++iter_region)
      {
         m_g4regions.emplace_back(new G4Region((*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume()->GetDaughter(iter_region)->GetName()+"_fastsim"));
         m_g4regions[iter_region]->AddRootLogicalVolume((*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume()->GetDaughter(iter_region)->GetLogicalVolume());
         if(m_g4regions.back()->GetName().find("Tracker") != std::string::npos)
         {
            m_models.emplace_back(new FastSimModelTest(m_g4regions.back()->GetName(),m_g4regions.back(),m_smearToolName));
            info()<<"Attaching a Fast Simulation Model to the region "<<m_g4regions.back()->GetName()<<endmsg;
         }
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
      error() << "Unable to initialize GEANT correctly." << endmsg;
      return StatusCode::FAILURE;
   }
}

StatusCode Geant4Simulation::execute() {
   //read event
   auto hepmc_event = m_eventhandle.get();
   if ( !hepmc_event->is_valid() )
   {
      error() << "Unable to get a HepMC event" << endmsg;
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

G4Event* Geant4Simulation::HepMC2G4(const HepMC::GenEvent* aHepMC_event) const
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

