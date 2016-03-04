#include "SimG4Common/SingleParticleGeneratorAction.h"
#include <CLHEP/Random/RandFlat.h>

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4HEPEvtInterface.hh"
#include "G4RunManager.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"


SingleParticleGeneratorAction::SingleParticleGeneratorAction()
{
	std::cout<<" SingleParticleGeneratorAction() "<<std::endl;
	particle="geantino";
	SetEnergyRange(1*GeV,1*TeV);
	SetEtaRange(-5.,5.);
	SetPhiRange();
	SetVertexPosition(0,0,0);
}
	
void SingleParticleGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    std::cout<<" GeneratePrimaries() "<<std::endl;
   
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particleDef = particleTable->FindParticle(particle);
    std::cout<<" particle definition "<<particleDef<<"+++"<<particle<<std::endl;
    G4double mass=0;
    if (particle!="geantino")
    	mass=particleDef->GetPDGMass();
    // std::cout << "particle mass = "<<mass<<std::endl;
        
    double particleEnergy = CLHEP::RandFlat::shoot(eMin,eMax);
    
    // std::cout << "particle energy = "<<particleEnergy<<std::endl;
	
    double eta = CLHEP::RandFlat::shoot(etaMin,etaMax);
    double phi = CLHEP::RandFlat::shoot(phiMin,phiMax);
    
    // std::cout << "particle eta,phi  = "<<eta<<" "<<phi<<std::endl;
    
    double theta = std::atan(std::exp(-eta))*2.;
    double randomX = std::sin(theta)*std::cos(phi);
    double randomY = std::sin(theta)*std::sin(phi);
    double randomZ = std::cos(theta);
  
    G4ThreeVector particleDir = G4ThreeVector(randomX,randomY,randomZ);
    G4ThreeVector particlePosition=G4ThreeVector(vertX,vertY,vertZ);
    
    G4PrimaryVertex* vertex=new G4PrimaryVertex(particlePosition,0.);
    G4PrimaryParticle* part=new G4PrimaryParticle(particleDef);
    
    part->SetMass(mass);
    part->SetKineticEnergy(particleEnergy);
    part->SetMomentumDirection(particleDir);
    part->SetCharge(particleDef->GetPDGCharge());
    
    vertex->SetPrimary(part);
    anEvent->AddPrimaryVertex(vertex);
    
}
	
void SingleParticleGeneratorAction::SetParticleType(std::string particleName)
{
 	particle=particleName;
}
void SingleParticleGeneratorAction::SetEnergyRange(double e1,double e2)
{
	eMin=e1;
	eMax=e2;
}
void SingleParticleGeneratorAction::SetEtaRange(double eta1,double eta2)
{
	etaMin=eta1;
	etaMax=eta2;
}
void SingleParticleGeneratorAction::SetPhiRange(double phi1,double phi2)
{
	phiMin=phi1;
	phiMax=phi2;
}
void SingleParticleGeneratorAction::SetVertexPosition(double vX,double vY,double vZ)
{
	vertX=vX;
	vertY=vY;
	vertZ=vZ;
}
