//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// based on G4 examples/extended/eventgenerator/HepMC/HepMCEx01/src/HepMCG4Interface.cc
//

#include "FCCHepMCInterface.hh"
#include "FCCPrimaryParticleInformation.hh"
#include "G4RunManager.hh"
#include "G4LorentzVector.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4TransportationManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
FCCHepMCInterface::FCCHepMCInterface(): fHepMCEvent(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
FCCHepMCInterface::~FCCHepMCInterface()
{
   delete fHepMCEvent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool FCCHepMCInterface::CheckVertexInsideWorld (const G4ThreeVector& aPosition) const
{
   G4Navigator* navigator= G4TransportationManager::GetTransportationManager()-> GetNavigatorForTracking();

   G4VPhysicalVolume* world= navigator-> GetWorldVolume();
   G4VSolid* solid= world-> GetLogicalVolume()-> GetSolid();
   EInside qinside= solid-> Inside(aPosition);

   if( qinside != kInside) return false;
   else return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void FCCHepMCInterface::HepMC2G4(const HepMC::GenEvent* aHepMCEvent, G4Event* aG4Event)
{

   G4cout << "Default units: " << HepMC::Units::name(HepMC::Units::default_momentum_unit())
          << " " << HepMC::Units::name(HepMC::Units::default_length_unit()) << G4endl;
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
      if (! CheckVertexInsideWorld(xvtx.vect()*mm)) continue;

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
         //HepMC::FourVector vert= (*vpitr)->production_vertex()->position();
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
   G4cout<<" HepMC Event has "<<aHepMCEvent->particles_size()<< " particles"<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMC::GenEvent* FCCHepMCInterface::GenerateHepMCEvent()
{
   HepMC::GenEvent* event= new HepMC::GenEvent();
   return event;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void FCCHepMCInterface::GeneratePrimaryVertex(G4Event* aEvent)
{
   // delete previous event object
   delete fHepMCEvent;

   // generate next event
   fHepMCEvent= GenerateHepMCEvent();
   if(! fHepMCEvent)
   {
      G4cout << "HepMCInterface: no generated particles. run terminated..."
             << G4endl;
      G4RunManager::GetRunManager()-> AbortRun();
      return;
   }
   HepMC2G4(fHepMCEvent, aEvent);
}
