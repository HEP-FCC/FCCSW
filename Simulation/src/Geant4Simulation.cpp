#include "Geant4Simulation.h"

#include "GeantFast/FCCDetectorConstruction.hh"
#include "GeantFast/FCCActionInitialization.hh"
#include "GeantFast/FCCPrimaryParticleInformation.hh"
#include "G4GDMLParser.hh"

#include "FTFP_BERT.hh"
#include "DataObjects/HepMCEntry.h"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

DECLARE_COMPONENT(Geant4Simulation)

Geant4Simulation::Geant4Simulation(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc)
{
   declareInput("hepmcevent", m_eventhandle);
   // declareInput("g4detector", m_g4detector);
   // declareOutput("particles", m_recphandle);
}

StatusCode Geant4Simulation::initialize() {
   GaudiAlgorithm::initialize();
   m_runManager = new G4RunManager;
   // load physics list
   m_runManager->SetUserInitialization(new FTFP_BERT);
   // take geometry
   ///.... from Service - check with Julia code, currently...
   //m_runManager->SetUserInitialization(new B4DetectorConstruction);

  // Set mandatory initialization classes
  //
   G4GDMLParser parser;
   parser.Read("FCCFullDetector.gdml");
   FCCDetectorConstruction* detConstruction = new FCCDetectorConstruction(parser.GetWorldVolume());
  m_runManager->SetUserInitialization(detConstruction);
  m_runManager->SetUserInitialization(new FCCActionInitialization);

   // const G4GDMLAuxMapType* auxmap = parser.GetAuxMap();
   // FCCFastSimGeometry FastSimGeometry(auxmap);
  

   // initialization
   m_runManager->ConstructScoringWorlds();
   m_runManager->Initialize();
   m_runManager->RunInitialization();

	return StatusCode::SUCCESS;
}

StatusCode Geant4Simulation::execute() {
   //read event
   const HepMC::GenEvent* hepmc_event = m_eventhandle.get()->getEvent();
   G4Event* geantEvent = new G4Event();
   HepMC2G4(hepmc_event, geantEvent);

   // run geant
   G4EventManager* eventManager = G4EventManager::GetEventManager();
   //as in  G4RunManager::ProcessOneEvent
   eventManager->ProcessOneEvent(geantEvent);
   //m_runManager->currentEvent = geantEvent;
   m_runManager->AnalyzeEvent(geantEvent);
   // m_runManager->UpdateScoring();
   //m_runManager->TerminateOneEvent();


   // ParticleCollection* particles = new ParticleCollection();
   // m_recphandle.put(particles);
   //delete geant_event;
   return StatusCode::SUCCESS;
}

StatusCode Geant4Simulation::finalize() {
   m_runManager->RunTermination();
   delete  m_runManager;
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

