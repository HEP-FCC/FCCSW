#ifndef SIMG4COMMON_SingleParticleGeneratorAction_H
#define SIMG4COMMON_SingleParticleGeneratorAction_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "G4Event.hh"
#include "G4VPrimaryGenerator.hh"

class G4ParticleGun;

class SingleParticleGeneratorAction: public G4VUserPrimaryGeneratorAction {
public:
	SingleParticleGeneratorAction();
	~SingleParticleGeneratorAction() {;}
	
	void GeneratePrimaries(G4Event* anEvent);
	
	void SetParticleType(std::string particleName);
	void SetEnergyRange(double eMin,double eMax);
	void SetEtaRange(double etaMin,double etaMax);
	void SetPhiRange(double phiMin=0,double phiMax=2*M_PI);
	void SetVertexPosition(double vX,double vY,double vZ);
	
private:
	double eMin,eMax;
	double etaMin,etaMax;
	double phiMin,phiMax;
	std::string particle;
	
	double vertX,vertY,vertZ;
};

#endif
