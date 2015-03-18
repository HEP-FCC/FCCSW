#include "HepMCToGeantConverter.h"
#include "G4Event.hh"

#include "G4LorentzVector.hh"

DECLARE_COMPONENT(HepMCToGeantConverter)

HepMCToGeantConverter::HepMCToGeantConverter(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc)
{
  declareInput("hepmc", m_hepmchandle);
  declareOutput("g4event", m_eventhandle);
}

StatusCode HepMCToGeantConverter::initialize() {
  return GaudiAlgorithm::initialize();
}

StatusCode HepMCToGeantConverter::execute() {
   HepMCEntry* theEvent = m_hepmchandle.get();
   const HepMC::GenEvent* event = theEvent->getEvent();
   G4Event* g4ev = new G4Event();
   HepMC2G4(event, g4ev);
   GeantEventEntry* entry = new GeantEventEntry();
   entry->setEvent(g4ev);
   m_eventhandle.put(entry);
   return StatusCode::SUCCESS;
}

StatusCode HepMCToGeantConverter::finalize() {
  return GaudiAlgorithm::finalize();
}


void HepMCToGeantConverter::HepMC2G4(const HepMC::GenEvent* aHepMCEvent, G4Event* aG4Event)
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

