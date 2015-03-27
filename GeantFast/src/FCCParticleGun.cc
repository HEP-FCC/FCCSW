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

#include "FCCParticleGun.hh"
#include "FCCPrimaryParticleInformation.hh"
#include "FCCEventInformation.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4EventManager.hh"
#include "FCCSmearer.hh"
#include "G4MuonPlus.hh"

FCCParticleGun::FCCParticleGun()
{
   G4ParticleGun::SetInitialValues();
}

FCCParticleGun::~FCCParticleGun()
{}

void FCCParticleGun::GeneratePrimaryVertex(G4Event* aEvent)
{
   G4double x0  = 0, y0  = 0, z0  = 0;
   G4double dx0 = 1, dy0 = 1, dz0 = 1;
   x0 = x0+ dx0*(G4UniformRand()-0.5);
   y0 = y0+ dy0*(G4UniformRand()-0.5);
   z0 = z0+ dz0*(G4UniformRand()-0.5);
   G4ThreeVector pos(x0,y0,z0);
   SetParticlePosition(pos);
   G4double px0  = 0, py0  = 0, pz0  = 0;
   G4double dpx0 = 1, dpy0 = 1, dpz0 = 1;
   px0 = px0+ dpx0*(G4UniformRand()-0.5);
   py0 = py0+ dpy0*(G4UniformRand()-0.5);
   pz0 = pz0+ dpz0*(G4UniformRand()-0.5);
   G4ThreeVector eP(px0,py0,pz0);
   SetParticleMomentumDirection(eP.unit());

   if(particle_definition==0) return;

   // create a new vertex
   G4PrimaryVertex* vertex = new G4PrimaryVertex(particle_position,particle_time);

   // create new primaries and set them to the vertex
   G4double mass =  particle_definition->GetPDGMass();
   for( G4int i=0; i<NumberOfParticlesToBeGenerated; i++ )
   {
      G4PrimaryParticle* particle =
         new G4PrimaryParticle(particle_definition);
      particle->SetKineticEnergy( particle_energy );
      particle->SetMass( mass );
      particle->SetMomentumDirection( particle_momentum_direction );
      particle->SetCharge( particle_charge );
      particle->SetPolarization(particle_polarization.x(),
                                particle_polarization.y(),
                                particle_polarization.z());
      particle->SetUserInformation( new FCCPrimaryParticleInformation(i, particle_definition->GetPDGEncoding() , (eP.unit())*particle_energy));
      vertex->SetPrimary( particle );
      G4double* params = FCCSmearer::Instance()->ComputeTrackParams(particle_charge, particle_energy*eP.unit(), pos );
       ((FCCPrimaryParticleInformation*)particle->GetUserInformation())->SetPerigeeMC(params);
   }
   aEvent->AddPrimaryVertex( vertex );
}
