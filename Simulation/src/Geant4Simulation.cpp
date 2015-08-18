#include "Geant4Simulation.h"

#include "GeantFast/FCCDetectorConstruction.hh"
#include "GeantFast/FCCActionInitialization.hh"
#include "GeantFast/FCCPrimaryParticleInformation.hh"
#include "GeantFast/FCCPhysicsList.hh"
#include "G4GDMLParser.hh"

#include "FTFP_BERT.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4ScoringManager.hh"

DECLARE_COMPONENT(Geant4Simulation)

Geant4Simulation::Geant4Simulation(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc), G4RunManager()
{
   declareInput("hepmcevent", m_eventhandle);
   // declareOutput("particles", m_recphandle);
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

   // load physics list
   SetUserInitialization(new FTFP_BERT);

   // take geometry
   SetUserInitialization(m_geoSvc->getGeant4Geo());

   // user action classes
   // SetUserInitialization(new FCCActionInitialization);

   Initialize();
   // as in G4RunManager::BeamOn
   numberOfEventToBeProcessed = 1;
   ConstructScoringWorlds();
   RunInitialization();

	return StatusCode::SUCCESS;
}

StatusCode Geant4Simulation::execute() {
   //read event
   auto hepmc_event = m_eventhandle.get();
   G4Event* geant_event = new G4Event();
   HepMC2G4(hepmc_event, geant_event);
   currentEvent = geant_event;

   // run geant
   //as in  G4RunManager::ProcessOneEvent
   eventManager->ProcessOneEvent( currentEvent);
   AnalyzeEvent(currentEvent);
   UpdateScoring();
   TerminateOneEvent();

   // ParticleCollection* particles = new ParticleCollection();
   // m_recphandle.put(particles);

   return StatusCode::SUCCESS;
}

StatusCode Geant4Simulation::finalize() {
   RunTermination();
   return GaudiAlgorithm::finalize();
}

void Geant4Simulation::HepMC2G4(const HepMC::GenEvent* aHepMCEvent, G4Event* aG4Event)
{
   for(HepMC::GenEvent::vertex_const_iterator vitr= aHepMCEvent->vertices_begin();
       vitr != aHepMCEvent->vertices_end(); ++vitr ) { // loop for vertex ...

      // real vertex?
      G4bool qvtx=false;
      for (HepMC::GenVertex::particle_iterator
              pitr= (*vitr)->particles_begin(HepMC::children);
           pitr != (*vitr)->particles_end(HepMC::children); ++pitr) {

         if (!(*pitr)->end_vertex() && (*pitr)->status()==1) {
            qvtx=true;
            break;
         }
      }
      if (!qvtx) continue;

      // check world boundary
      HepMC::FourVector pos= (*vitr)-> position();
      G4LorentzVector xvtx(pos.x(), pos.y(), pos.z(), pos.t());

      // create G4PrimaryVertex and associated G4PrimaryParticles
      G4PrimaryVertex* g4vtx=
         new G4PrimaryVertex(xvtx.x()*cm, xvtx.y()*cm, xvtx.z()*cm,
                             xvtx.t()*cm/c_light);

      for (HepMC::GenVertex::particle_iterator
              vpitr= (*vitr)->particles_begin(HepMC::children);
           vpitr != (*vitr)->particles_end(HepMC::children); ++vpitr) {

         if( (*vpitr)->status() != 1 ) continue;

         G4int pdgcode= (*vpitr)-> pdg_id();
         pos= (*vpitr)-> momentum();
         G4LorentzVector p(pos.px(), pos.py(), pos.pz(), pos.e());
         G4PrimaryParticle* g4prim=
            new G4PrimaryParticle(pdgcode, p.x()*GeV, p.y()*GeV, p.z()*GeV);
         g4prim->SetUserInformation(new FCCPrimaryParticleInformation(
                                       (*vpitr)->barcode(),
                                       pdgcode,
                                       G4ThreeVector(p.x(), p.y(), p.z())));
         g4vtx-> SetPrimary(g4prim);
      }
      aG4Event-> AddPrimaryVertex(g4vtx);
   }
}

