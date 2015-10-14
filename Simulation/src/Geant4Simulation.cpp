#include "Geant4Simulation.h"

// Geant4
#include "FTFP_BERT.hh"
#include "G4SystemOfUnits.hh"
#include "G4RegionStore.hh"
#include "G4VModularPhysicsList.hh"

// FCCSW
#include "GeantFast/FastSimPhysics.h"
#include "GeantFast/FastSimModelTest.h"
#include "GeantFast/ParticleInformation.h"
#include "GeantFast/TrackingAction.h"
#include "GeantFast/Units.h"

DECLARE_COMPONENT(Geant4Simulation)

Geant4Simulation::Geant4Simulation(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc), G4RunManager(), m_type(SimType::FULL)
{
   declareInput("genparticles", m_genphandle);
   declareOutput("particles", m_recphandle);
   declareOutput("particleassociation", m_partassociationhandle);
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
        m_smearTool = tool<ISmearingTool>(m_smearToolName);
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

   // Attach UserActions
   // G4VUserTrackingAction deleted in ~G4TrackingManager()
   G4RunManager::SetUserAction(new TrackingAction);
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
   G4RunManager::currentEvent = EDM2G4();
   if ( !G4RunManager::currentEvent )
   {
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

G4Event* Geant4Simulation::EDM2G4()
{
   // Event will be passed to G4RunManager and be deleted in G4RunManager::RunTermination()
   G4Event* g4_event = new G4Event();
   // Creating EDM collections
   const MCParticleCollection* mcparticles = m_genphandle.get();
   ParticleCollection* particles = new ParticleCollection();
   ParticleMCAssociationCollection* associations = new ParticleMCAssociationCollection();
   // Adding one particle per one vertex => vertices repeated
   for(const auto& mcparticle : *mcparticles)
   {
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

