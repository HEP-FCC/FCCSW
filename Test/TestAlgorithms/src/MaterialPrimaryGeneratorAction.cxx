//
//  MaterialPrimaryGeneratorAction.cxx
//  
//
//  Created by Julia Hrdinka on 19/05/15.
//
//


#include "TestAlgorithms/MaterialPrimaryGeneratorAction.h"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"
#include "G4RandomDirection.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MaterialPrimaryGeneratorAction* MaterialPrimaryGeneratorAction::fgInstance = 0;

MaterialPrimaryGeneratorAction::MaterialPrimaryGeneratorAction(
                                                           const G4String& particleName,
                                                           G4double energy,
                                                           G4ThreeVector position,
                                                           G4ThreeVector momentumDirection)
: G4VUserPrimaryGeneratorAction(),
fParticleGun(0),
m_dir()
{
    fgInstance = this;
    G4int nofParticles = 1;
    fParticleGun  = new G4ParticleGun(nofParticles);
    // default particle kinematic
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName);
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(energy);
    fParticleGun->SetParticlePosition(position);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MaterialPrimaryGeneratorAction::~MaterialPrimaryGeneratorAction()
{
    fgInstance = 0;
    delete fParticleGun;
}

MaterialPrimaryGeneratorAction* MaterialPrimaryGeneratorAction::Instance()
{
    // Static acces function via G4RunManager
    
    return fgInstance;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MaterialPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    // this function is called at the begining of event
    //hier random generator
    //mit G4UnifotmRand()

    G4int sign  = 1;
    if (G4UniformRand()>0.5) {
        sign = -1;
    }
   
    G4double x0 = 2.*(G4UniformRand()-0.5);
    G4double y0 = 2.*(G4UniformRand()-0.5);
    G4double z0 = 2.*(G4UniformRand()-0.5);

    
   // G4double theta  = (2.8133-0.3282)*G4UniformRand()+0.3282;
  //  G4double phi    = sign*M_PI*G4UniformRand();
    
   // m_dir = G4ThreeVector(cos(phi)*sin(theta),sin(phi)*sin(theta),cos(theta));
 
//    m_dir = G4ThreeVector(-0.146748,0.11085,0.982943);
    
    
    m_dir = G4ThreeVector(x0,y0,z0);
    
//    m_dir = G4ThreeVector(1.,0.,0.);
    
/*    G4cout << "___________________________________" << G4endl;
    G4cout << " new direction: " << m_dir << " eta: " << m_dir.eta() << G4endl;
    G4cout << "___________________________________" << G4endl;*/
 //   m_dir = G4ThreeVector(0.08393,0.,0.99647);

 //   fParticleGun->SetParticleMomentumDirection(G4RandomDirection());
    fParticleGun->SetParticleMomentumDirection(m_dir);
    
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
