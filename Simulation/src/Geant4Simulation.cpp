#include "Geant4Simulation.h"

#include "GeantFast/B4DetectorConstruction.hh"
#include "GeantFast/H03PrimaryGeneratorAction.hh"
#include "GeantFast/B4RunAction.hh"
#include "GeantFast/B4aEventAction.hh"
#include "GeantFast/B4aSteppingAction.hh"

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
  B4DetectorConstruction* detConstruction = new B4DetectorConstruction();
  m_runManager->SetUserInitialization(detConstruction);

  B4aEventAction* eventAction = new B4aEventAction();
  m_runManager->SetUserAction(eventAction);
  m_runManager->SetUserAction(new B4RunAction());
  m_runManager->SetUserAction(new H03PrimaryGeneratorAction());
  B4aSteppingAction* steppingAction 
    = new B4aSteppingAction(detConstruction, eventAction);
  m_runManager->SetUserAction(steppingAction);

   // initialization
   m_runManager->Initialize();
   m_runManager->RunInitialization();

	return StatusCode::SUCCESS;
}

StatusCode Geant4Simulation::execute() {
   //read event
   const HepMC::GenEvent* hepmc_event = m_eventhandle.get()->getEvent();
   G4Event* geant_event = new G4Event();
   HepMC2G4(hepmc_event, geant_event);

   // // run geant
   // G4EventManager* eventManager = G4EventManager::GetEventManager();
   // eventManager->ProcessOneEvent(geant_event);

   // ParticleCollection* particles = new ParticleCollection();
   // m_recphandle.put(particles);
   delete geant_event;
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
         new G4PrimaryVertex(xvtx.x()*mm, xvtx.y()*mm, xvtx.z()*mm,
                             xvtx.t()*mm/c_light);

      for (HepMC::GenVertex::particle_iterator
              vpitr= (*vitr)->particles_begin(HepMC::children);
           vpitr != (*vitr)->particles_end(HepMC::children); ++vpitr) {

         if( (*vpitr)->status() != 1 ) continue;

         G4int pdgcode= (*vpitr)-> pdg_id();
         pos= (*vpitr)-> momentum();
         G4LorentzVector p(pos.px(), pos.py(), pos.pz(), pos.e());
         G4PrimaryParticle* g4prim=
            new G4PrimaryParticle(pdgcode, p.x()*GeV, p.y()*GeV, p.z()*GeV);
         // g4prim->SetUserInformation(new FCCPrimaryParticleInformation(
         //                               (*vpitr)->barcode(),
         //                               pdgcode,
         //                               G4ThreeVector(p.x(), p.y(), p.z())));
         g4vtx-> SetPrimary(g4prim);
      }
      aG4Event-> AddPrimaryVertex(g4vtx);
   }
}

